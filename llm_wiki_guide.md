# LLM Wiki — LLM을 능동적 지식 유지 시스템으로

> 작성일: 2026-04-19

---

## 목차

- [1. 배경: 지식은 왜 사라지는가](#1-배경-지식은-왜-사라지는가)
- [2. LLM Wiki란?](#2-llm-wiki란)
- [3. 3계층 아키텍처](#3-3계층-아키텍처)
- [4. 핵심 오퍼레이션](#4-핵심-오퍼레이션)
- [5. RAG와의 비교](#5-rag와의-비교)
- [6. 하네스 엔지니어링과의 관계](#6-하네스-엔지니어링과의-관계)
- [7. 관련 시스템](#7-관련-시스템)
- [8. 구현 설계 패턴](#8-구현-설계-패턴)
- [9. 한계와 미해결 과제](#9-한계와-미해결-과제)
- [참고자료](#참고자료)

---

## 1. 배경: 지식은 왜 사라지는가

LLM을 사용하다 보면 반복적인 비효율을 경험한다. 어제 LLM과 함께 분석한 내용, 어렵게 도달한 결론, 정제된 이해—이 모든 것이 대화 세션이 끝나면 사라진다. 다음 세션에서 같은 주제를 다시 꺼내면 LLM은 처음부터 다시 시작한다.

RAG(Retrieval-Augmented Generation)는 이 문제를 부분적으로 해결했다. 원본 문서를 저장해두고, 질문할 때마다 관련 문서를 검색해 컨텍스트에 담는다. 그러나 RAG도 근본적인 한계를 갖는다: **합성된 이해(synthesized understanding)** 는 저장하지 못한다.

```
[RAG의 한계]

1회 질의: 문서 A, B, C 검색 → LLM이 합성 → 답변
2회 질의: 문서 A, B, C 검색 → LLM이 다시 합성 → 답변
3회 질의: 문서 A, B, C 검색 → LLM이 또 합성 → 답변

→ 지식이 누적되지 않는다. 매번 같은 비용으로 같은 작업을 반복.
→ 이전 질의에서 발견한 모순, 도달한 결론, 정제된 이해가 사라진다.
```

Andrej Karpathy는 2026년 4월, 이 문제를 정면으로 지적하며 새로운 패턴을 제안했다.

> "좋은 답변이 채팅 로그 속으로 사라진다. 쿼리는 쌓이지 않는다."

---

## 2. LLM Wiki란?

**LLM Wiki**는 LLM을 수동적 쿼리 엔진이 아닌, 능동적으로 지식을 구축하고 유지하는 큐레이터로 활용하는 패턴이다.

핵심 아이디어는 단순하다: **LLM이 새로운 정보를 처리할 때마다, 그 이해를 구조화된 마크다운 Wiki로 기록하고 업데이트한다.** 이후 질문에 대해서는 원본 문서를 다시 분석하는 대신, 이미 합성되고 정제된 Wiki를 참조한다.

```
[LLM Wiki의 핵심 루프]

새 소스 입력 → Ingest Agent → Wiki 업데이트 (이해를 기록)
                                    ↓
질문 → Query Agent → Wiki 검색 → 합성 → 답변
                                    ↑
                              Lint Agent (주기적 검증)
```

RAG가 "LLM에게 원본을 보여주는 것"이라면, LLM Wiki는 "LLM이 미리 공부한 노트를 만들어두는 것"이다.

### 왜 지금인가

2026년 초, LLM의 추론 능력과 문서 이해 품질이 높아지면서 이 패턴이 실용적이 되었다. Karpathy의 제안은 이미 여러 연구팀이 탐색하던 방향(STORM, WikiLLM 등)을 하나의 명확한 패턴으로 정리했고, AI 엔지니어링 커뮤니티에서 빠르게 확산되었다.

---

## 3. 3계층 아키텍처

LLM Wiki는 세 개의 계층으로 구성된다.

![LLM Wiki 3계층 아키텍처: Raw Sources → Wiki → Schema 구조](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*XGv7Rz9IuBNx5YZQT6KFJA.png)

> *출처: Medium — [What is LLM Wiki Pattern?](https://medium.com/@tahirbalarabe2/what-is-llm-wiki-pattern-persistent-knowledge-with-llm-wikis-3227f561abc1)*

### 3.1 Layer 1: Raw Sources (원본 소스)

**정의**: 시스템에 입력되는 모든 원본 자료. PDF, 웹 페이지, 코드베이스, 커밋 로그, 논문, 회의록 등.

**핵심 특성**: **불변(immutable)**. 한 번 인제스트 대상으로 등록되면 수정하지 않는다. 원본 자료의 무결성을 보장하기 위해 append-only 방식으로 관리한다.

**실제 구현**: 파일 경로 목록 + 인제스트 처리 상태 추적 (`ingest_log.json` 등)

```
raw_sources/
  papers/
    storm_naacl2024.pdf       # 처리 완료
    memit_iclr2023.pdf        # 처리 완료
  web/
    karpathy_llm_wiki.html    # 처리 완료
  code/
    storm_repo/               # 처리 중
```

Raw Sources는 지식의 **증거(evidence)** 다. Wiki의 내용이 의심스러울 때 돌아올 수 있는 원점이다.

### 3.2 Layer 2: The Wiki

**정의**: LLM이 능동적으로 작성하고 유지보수하는 마크다운 파일 집합. 시스템의 핵심.

**핵심 특성**: **가변(mutable), 구조화, LLM 유지**. Raw Sources와 달리 Wiki는 계속 진화한다. 새 소스가 인제스트될 때마다, Lint가 실행될 때마다 내용이 갱신된다.

Wiki는 단순한 요약이 아니다. LLM이 여러 소스에서 패턴을 파악하고, 모순을 해결하고, 관계를 정리한 **이해의 구조화된 표현**이다.

```
wiki/
  index.md                    # 전체 Wiki 구조 개요
  concepts/
    llm_wiki.md               # LLM Wiki 패턴 개요
    rag.md                    # RAG 개념 및 한계
    context_engineering.md    # 컨텍스트 엔지니어링
  systems/
    storm.md                  # STORM 시스템 분석
    memit.md                  # MEMIT 알고리즘
  contradictions/
    rag_vs_wiki_tradeoffs.md  # 해결 중인 모순들
  changelog.md                # 인제스트 이력
```

### 3.3 Layer 3: Schema (편집 정책)

**정의**: Wiki를 어떻게 구성하고 유지할지에 대한 규칙. LLM에게 주어지는 "편집 정책".

**핵심 특성**: **안정적(stable)**. Schema는 자주 바뀌지 않는다. Ingest/Lint 시마다 LLM에게 시스템 프롬프트로 전달되어 Wiki의 일관성을 보장한다.

Schema 없이는 여러 번의 Ingest가 서로 다른 구조의 Wiki 페이지를 만들어낸다. Schema는 이 일관성을 강제하는 계약이다.

```markdown
# schema.md (예시)

## 페이지 명명 규칙
- 파일명: kebab-case, 영어 소문자
- 최대 50자

## 모든 개념 페이지의 필수 섹션
1. ## 정의 — 핵심 개념을 2-3문장으로
2. ## 핵심 특성 — 번호 없는 목록, 최대 7항목
3. ## 관련 개념 — 다른 Wiki 페이지 링크
4. ## 출처 — 인용한 Raw Source 목록

## 모순 처리 정책
- 해결 불가능한 모순은 contradictions/ 디렉토리로 분리
- 최신 소스(날짜 기준) 우선, 단 이전 소스를 삭제하지 않음

## 업데이트 granularity
- 기존 페이지: 섹션 단위로 업데이트 (전체 재작성 지양)
- 신규 개념: 새 페이지 생성
```

---

## 4. 핵심 오퍼레이션

LLM Wiki는 세 가지 오퍼레이션으로 동작한다.

### 4.1 Ingest (인제스트)

새로운 Raw Source를 처리하여 Wiki를 업데이트하는 오퍼레이션.

```
[Ingest 플로우]

새 소스 입력
  → Ingest Agent: 문서 파싱
  → 관련 Wiki 페이지 식별 (Wiki에 대한 의미 검색)
  → Schema 참조: 어떤 페이지에 무엇을 쓸지 결정
  → Wiki 업데이트:
      - 기존 페이지: 새 정보 병합
      - 신규 개념: 새 페이지 생성
      - 모순 발견: contradictions/ 에 기록
  → Ingest Log 갱신
```

**핵심 과제**:
- **중복 처리**: 같은 정보가 여러 소스에 있을 때 — Schema에 정의된 병합 규칙 적용
- **충돌 처리**: 두 소스가 상충하는 정보를 줄 때 — 즉시 해결 불가라면 `contradictions/` 로 분리
- **점진적 업데이트**: 전체 재작성 대신 변경된 부분만 업데이트

### 4.2 Query (쿼리)

사용자 질문에 답하는 오퍼레이션. LLM Wiki의 핵심 가치가 발현되는 지점.

```
[Query 플로우]

사용자 질문
  → Query Agent: 관련 Wiki 페이지 식별 (의미 검색)
  → Wiki 컨텍스트 로드
  → LLM 합성 → 답변
  → (선택적) 새 통찰 발생 시 Wiki 업데이트 트리거
```

**RAG Query와의 차이**:
- RAG: 원본 문서 검색 → 매번 처음부터 합성
- LLM Wiki Query: 이미 합성된 Wiki 검색 → 빠르고 일관된 답변

Wiki가 이미 여러 소스 간 모순을 해결했기 때문에, Query 결과가 더 일관성 있고 신뢰할 수 있다.

### 4.3 Lint (린트)

Wiki 내부의 모순, 오래된 정보, 스키마 위반을 자동으로 찾아내는 오퍼레이션.

```
[Lint 플로우]

스케줄 또는 Ingest 완료 후 트리거
  → Lint Agent: 모든 Wiki 페이지 순회
      - 논리적 일관성 검사 (LLM 기반)
      - Schema 준수 여부 검사 (규칙 기반)
      - Raw Sources와의 최신성 비교 (날짜 기반)
  → Lint Report 생성
  → 자동 수정 가능한 항목: 즉시 수정
  → 수동 검토 필요 항목: 보고서에 플래그
```

**Lint가 찾는 문제들**:

| 문제 유형 | 설명 | 처리 방법 |
|-----------|------|-----------|
| 모순(Contradiction) | 두 페이지가 상반된 사실 주장 | `contradictions/` 로 분리 |
| 오래된 정보(Staleness) | 최신 소스와 충돌하는 Wiki 내용 | 업데이트 트리거 |
| 고아 링크(Orphan reference) | 존재하지 않는 Wiki 페이지 참조 | 링크 제거 또는 페이지 생성 |
| 스키마 위반 | 필수 섹션 누락, 명명 규칙 위반 | 자동 수정 |

코드의 `sparse`나 `checkpatch.pl`처럼, Lint는 Wiki를 "실행"하지 않고 정적으로 검증한다.

---

## 5. RAG와의 비교

LLM Wiki는 RAG를 대체하는 것이 아니라, 특정 사용 패턴에서 RAG의 한계를 극복하는 보완적 패턴이다.

| 항목 | RAG | LLM Wiki |
|------|-----|----------|
| **지식 저장 방식** | 원본 문서 유지 (변환 없음) | LLM이 합성한 Wiki 유지 |
| **Query 시 합성** | 매번 처음부터 합성 | 이미 합성된 내용 참조 |
| **지식 누적** | 문서 추가로 간접 누적 | Wiki 업데이트로 직접 누적 |
| **모순 처리** | 쿼리 시 즉흥적 해결 | Ingest/Lint 시 명시적 해결 |
| **지식 진화 추적** | 어려움 | Wiki 변경 이력(git)으로 추적 가능 |
| **원본 충실도** | 높음 (원본 그대로 보존) | 낮음 (LLM 해석이 개입) |
| **초기 구축 비용** | 낮음 | 높음 (Ingest 비용) |
| **반복 Query 비용** | 동일 (매번 검색 + 합성) | 낮음 (캐시된 Wiki 참조) |
| **적합한 도메인** | 다양한 소스, 빠른 프로토타입 | 좁고 깊은 전문 도메인 |

### 언제 무엇을 선택할까

```
도메인이 좁고 반복 질의가 많다
  → LLM Wiki 적합

소스가 자주 바뀐다 (실시간성 중요)
  → RAG 또는 LLM Wiki + 짧은 Lint 주기

빠른 프로토타입 필요
  → RAG 시작, 성숙 후 LLM Wiki 고려

지식의 진화와 감사 추적이 중요하다
  → LLM Wiki (git 이력)
```

---

## 6. 하네스 엔지니어링과의 관계

LLM Wiki는 추상적인 "에이전트 하네스"가 **지식 관리**라는 구체적 문제에 적용된 사례다. AI 엔지니어링 진화의 관점에서 보면:

```
프롬프트 엔지니어링  → "Wiki에 어떻게 질문할까?"
컨텍스트 엔지니어링 → "Wiki의 어떤 페이지를 컨텍스트에 담을까?"
하네스 엔지니어링   → "Wiki를 어떻게 자율적으로 유지보수할까?" ← LLM Wiki
```

### 하네스 컴포넌트별 대응

[ai_engineering_evolution.md](./ai_engineering_evolution.md)에서 정의한 하네스의 핵심 구성요소가 LLM Wiki에서 어떻게 구현되는지:

| 하네스 컴포넌트 | LLM Wiki에서의 역할 |
|----------------|-------------------|
| **훅(Hooks)** | `PostIngest` 훅: Ingest 완료 후 Lint 자동 트리거 |
| **퍼미션 시스템** | Ingest/Lint Agent는 Wiki 쓰기 권한, Query Agent는 읽기 전용 |
| **서브에이전트 조율** | Ingest, Query, Lint Agent를 오케스트레이터가 조율 |
| **상태 & 메모리** | Wiki 자체가 세션 간 영속 메모리 |
| **에러 복구 로직** | Lint 실패 시 Planner가 해당 Wiki 페이지 재작성 결정 |
| **옵저버빌리티** | Ingest log, Lint report, Wiki git history |

### P/G/E 패턴과의 연결

[ai_engineering_evolution.md](./ai_engineering_evolution.md)의 Planner/Generator/Evaluator 패턴이 LLM Wiki Ingest 플로우에 그대로 적용된다:

```
LLM Wiki Ingest = P/G/E 패턴의 구체적 구현

Planner   → 어떤 Wiki 페이지를 업데이트할지, 어떤 구조로 합성할지 결정
Generator → 실제 Wiki 페이지 내용 작성/수정 (Schema 기반)
Evaluator → Lint: Schema 준수 여부 + 기존 페이지와의 모순 검증
            → 실패 시 Generator에게 재작성 요청 (피드백 루프)
```

이 연결은 단순한 비유가 아니다. LLM Wiki를 실제 구현할 때, P/G/E 오케스트레이션 프레임워크(LangGraph, AutoGen 등) 위에 Ingest 파이프라인을 구축하는 것이 자연스러운 설계다.

---

## 7. 관련 시스템

### 7.1 STORM (Stanford, NAACL 2024)

**논문**: "Assisting in Writing Wikipedia-like Articles From Scratch with Large Language Models" (Shao et al., NAACL 2024)

STORM은 LLM Wiki와 목표를 공유하지만, 접근 방식이 다르다.

**핵심 아이디어**: 특정 토픽에 대해 다양한 관점을 가진 가상 전문가 에이전트들을 생성하고, 이들이 서로 인터뷰 형식으로 대화하며 포괄적인 Wikipedia 스타일의 글을 자동 작성한다.

```
[STORM 플로우]

토픽 입력
  → 다양한 관점의 가상 전문가 에이전트 생성
  → 에이전트 간 인터뷰: "당신의 관점에서 이 토픽에 대해 어떤 질문이 중요한가?"
  → 포괄적인 질문 집합 생성
  → 신뢰할 수 있는 소스에서 답변 수집
  → 구조화된 Wikipedia 스타일 글 작성
```

![STORM 시스템 아키텍처: 다중 관점 에이전트 기반 Wikipedia 글 자동 생성](https://raw.githubusercontent.com/stanford-oval/storm/main/assets/storm_overview.png)

> *출처: Stanford OVAL — [STORM GitHub](https://github.com/stanford-oval/storm)*

**STORM vs LLM Wiki**:

| 항목 | STORM | LLM Wiki |
|------|-------|----------|
| **목적** | 단일 글 생성 (one-shot) | 지속적 지식 유지보수 |
| **출력** | 완성된 Wikipedia 글 | 진화하는 Wiki 파일 집합 |
| **시간축** | 단발성 | 지속적 |
| **소스 처리** | 질의 응답 기반 수집 | 직접 문서 인제스트 |

STORM의 다중 관점 에이전트 기법은 LLM Wiki의 Ingest 단계에 통합할 수 있다. 새 소스를 "여러 관점에서 동시에 분석"하면 더 풍부한 Wiki를 생성할 수 있다.

### 7.2 WikiLLM / MEMIT (가중치 편집 방식)

**논문**: "Mass-Editing Memory in a Transformer" (Meng et al., ICLR 2023)

MEMIT은 외부 Wiki 파일 대신, 새로운 사실을 **모델 가중치(weights)에 직접 삽입**하는 접근이다. 특정 레이어의 특정 뉴런을 수정하여 모델의 "기억"을 업데이트한다.

**Karpathy 방식 vs MEMIT 비교**:

| 항목 | LLM Wiki (외부 파일) | MEMIT (가중치 편집) |
|------|---------------------|-------------------|
| **지식 위치** | 외부 Markdown 파일 | 모델 파라미터 |
| **검사 가능성** | 파일을 열면 확인 가능 | 가중치 해석 불가 |
| **확장성** | 파일 수에 비례 | 편집 수 증가 시 성능 저하 |
| **롤백** | `git revert` | 사실상 불가 |
| **비용** | LLM API 비용 | 파인튜닝 수준의 컴퓨팅 |
| **현실적 사용** | 높음 | 연구 단계 |

MEMIT은 개념적으로 흥미롭지만, 2026년 기준으로 실용적인 LLM Wiki 구현에는 Karpathy 방식(외부 마크다운 파일)이 압도적으로 현실적이다.

---

## 8. 구현 설계 패턴

### 8.1 최소 구현 스케치

```python
class LLMWiki:
    def __init__(self, wiki_dir: str, schema_path: str):
        self.wiki_dir = wiki_dir
        self.schema = load_schema(schema_path)

    def ingest(self, source_path: str):
        content = read_source(source_path)
        related_pages = semantic_search(self.wiki_dir, content)
        
        for page in related_pages:
            updated = llm_merge(
                existing=read_page(page),
                new_content=content,
                schema=self.schema
            )
            write_page(page, updated)
        
        if new_concepts := extract_new_concepts(content, self.wiki_dir):
            for concept in new_concepts:
                new_page = llm_create_page(concept, content, self.schema)
                write_page(f"{self.wiki_dir}/concepts/{concept}.md", new_page)
        
        log_ingest(source_path)

    def query(self, question: str) -> str:
        related_pages = semantic_search(self.wiki_dir, question)
        context = [read_page(p) for p in related_pages]
        return llm_synthesize(question, context)

    def lint(self) -> LintReport:
        all_pages = load_all_pages(self.wiki_dir)
        return llm_check_consistency(all_pages, self.schema)
```

### 8.2 Wiki 파일 구조

```
project/
  raw_sources/              # Layer 1: 원본 소스 (불변)
    papers/
    web/
    code/
  ingest_log.json           # 처리 이력

  wiki/                     # Layer 2: Wiki (LLM 유지)
    index.md
    concepts/               # 개념별 페이지
    systems/                # 시스템/도구 분석
    entities/               # 인물, 조직, 프로젝트
    contradictions/         # 미해결 모순
    changelog.md            # 인제스트 이력

  schema.md                 # Layer 3: 편집 정책
```

### 8.3 Git을 이용한 Wiki 버전 관리

Wiki를 git 저장소로 관리하면 지식의 진화를 추적할 수 있다:

```bash
# 특정 페이지의 변경 이력
git log wiki/concepts/rag.md

# 최신 Ingest가 Wiki에 미친 영향
git diff HEAD~1 wiki/

# 잘못된 Ingest 결과 롤백
git revert <ingest-commit-hash>

# 두 시점의 Wiki 비교
git diff v1.0 v2.0 wiki/
```

이 접근은 단순한 파일 관리를 넘어, **지식의 감사 추적(audit trail)** 을 가능하게 한다. RAG 시스템에서는 불가능한 것이다.

### 8.4 Schema 설계 원칙

효과적인 Schema를 위한 5가지 원칙:

1. **명명 규칙 명시**: `kebab-case`, 최대 길이, 디렉토리 분류 기준
2. **필수 섹션 정의**: 모든 페이지가 가져야 하는 H2 헤더 목록
3. **인용 정책**: 사실 주장에 반드시 포함해야 하는 출처 형식
4. **모순 처리 절차**: 명시적 `contradictions/` 분리 vs 최신 정보 우선
5. **업데이트 granularity**: 섹션 수준 업데이트 vs 문장 수준 (대부분 섹션 수준 권장)

---

## 9. 한계와 미해결 과제

LLM Wiki는 강력한 패턴이지만, 2026년 4월 기준으로 여전히 실험적 아이디어 단계다. 솔직한 한계 분석이 필요하다.

### 9.1 환각의 영속화

RAG는 원본 문서를 그대로 보존하기 때문에, LLM이 잘못된 답변을 해도 소스는 정확하다. LLM Wiki에서는 Ingest 시 LLM이 생성한 잘못된 내용이 **Wiki에 고착화**될 수 있다.

> "garbage in, garbage out"의 영속 버전: 나쁜 소스 또는 나쁜 Ingest가 지식 베이스를 오염시킨다.

**완화 방법**: Lint의 강화, 인간 검토 포인트 추가, 중요 사실에 대한 소스 인용 강제

### 9.2 확장성 제한

Wiki 페이지가 수천 개가 되면:
- 관련 페이지 검색의 정확도 저하
- Lint 실행 비용: N 페이지 × M 비교 = 잠재적 O(N²) 비용
- Schema 위반이 누적될수록 일관성 보장이 어려워짐

### 9.3 Schema 진화의 어려움

도메인이 성장하면 Schema도 바뀌어야 한다. 그러나 Schema가 바뀌면 기존 Wiki 페이지가 모두 위반 상태가 된다. **Schema 마이그레이션 전략**이 필요하지만, 아직 표준화된 방법이 없다.

### 9.4 "합의"의 위험

Lint가 모순을 자동으로 해결할 때, LLM이 어느 쪽이 옳은지 결정한다. 이 결정에 LLM의 편향이 개입되어, **편향된 LLM이 편향된 "진실"을 Wiki에 기록**할 수 있다.

### 9.5 현재 상태 요약

| 항목 | 상태 |
|------|------|
| 패턴 성숙도 | 실험적 (2026년 4월 기준) |
| 표준 구현 | 없음 |
| 명확한 벤치마크 | 없음 |
| 커뮤니티 채택 | 초기 단계 |
| 도구 지원 | 제한적 |

LLM Wiki를 도입하기 전에, RAG로 시작하여 반복 질의 패턴이 명확해진 후 전환을 고려하는 것이 현실적인 접근이다.

---

## 참고자료

- [What is LLM Wiki Pattern?](https://medium.com/@tahirbalarabe2/what-is-llm-wiki-pattern-persistent-knowledge-with-llm-wikis-3227f561abc1) — Karpathy 제안의 명확한 설명
- [Why People Are Paying Attention to LLM Wiki](https://medium.com/@aristojeff/what-is-an-llm-wiki-and-why-are-people-paying-attention-to-it-b7e10617967d) — 커뮤니티 반응 분석
- [LLM Wiki Revolution: Andrej Karpathy's Idea](https://www.analyticsvidhya.com/blog/2026/04/llm-wiki-by-andrej-karpathy/) — 기술적 상세 설명
- [LLM Wiki Tutorial - Data Science Dojo](https://datasciencedojo.com/blog/llm-wiki-tutorial/) — 실습 튜토리얼
- [STORM: Automating Wikipedia Article Creation](https://arxiv.org/abs/2402.14207) — Shao et al., NAACL 2024
- [STORM GitHub Repository](https://github.com/stanford-oval/storm) — Stanford OVAL 그룹 구현체
- [MEMIT: Mass-Editing Memory in a Transformer](https://memit.baulab.info/) — Meng et al., ICLR 2023
- [WikiLLM GitHub](https://github.com/laramohan/wikillm) — MEMIT 기반 LLM Wiki 구현 예시
- [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents) — Anthropic, 하네스 엔지니어링 기반
- [What is LLM Wiki and How to Build It](https://www.mindstudio.ai/blog/andrej-karpathy-llm-wiki-knowledge-base-claude-code) — MindStudio, Claude Code 기반 구현 가이드
