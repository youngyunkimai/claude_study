# AI 엔지니어링 패러다임의 진화
## 프롬프트 엔지니어링 → 컨텍스트 엔지니어링 → 하네스 엔지니어링

> 작성일: 2026-04-01

---

## 1. 프롬프트 엔지니어링 (Prompt Engineering)

### 개념
LLM에게 **무엇을 물어볼지** 잘 작성하는 기술. 단일 텍스트 입력(프롬프트)을 정교하게 다듬어 원하는 출력을 얻는 데 집중.

### 핵심 기법
- Few-shot 예시 제공
- Chain-of-thought (단계적 사고 유도)
- 역할 지정 ("당신은 전문가입니다")
- 출력 형식 명시

### 한계
- **일회성**: 대화마다 처음부터 다시 작성
- **단순성**: 단일 입력/출력 구조에 최적화
- **상태 없음**: 이전 대화나 외부 정보를 체계적으로 다루지 못함
- 모델 성능이 높아질수록 "잘 물어보는 기술"의 차별성이 줄어듦

---

## 2. 컨텍스트 엔지니어링 (Context Engineering)

### 개념
모델의 컨텍스트 윈도우에 **무엇을 담을지** 설계하는 기술. 단순히 질문을 잘 쓰는 것을 넘어, 모델이 올바른 추론을 하도록 필요한 정보 전체를 조직하는 것.

### 핵심 요소
```
컨텍스트 윈도우 = 시스템 프롬프트
                + 관련 문서/코드 (RAG)
                + 대화 이력
                + 툴 정의 및 결과
                + 사용자 입력
```

### 핵심 기법
- **RAG (Retrieval-Augmented Generation)**: 필요한 문서를 검색해 컨텍스트에 삽입
- **메모리 관리**: 어떤 대화 이력을 요약/보존/삭제할지 결정
- **툴 결과 주입**: 함수 호출 결과를 컨텍스트에 포함
- **컨텍스트 압축**: 토큰 한계 안에서 정보 밀도 최적화

### Augmented LLM — 컨텍스트 엔지니어링의 구조

![Augmented LLM: LLM이 Retrieval(Query/Results), Tools(Call/Response), Memory(Read/Write) 세 가지 확장 레이어와 연결된 구조](https://cdn.sanity.io/images/4zrzovbb/website/d3083d3f40bb2b6f477901cc9a240738d3dd1371-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
>
> 단순한 LLM(In → LLM → Out)에 **Retrieval**(외부 지식), **Tools**(외부 액션), **Memory**(상태 저장)를 붙인 것이 컨텍스트 엔지니어링의 핵심 구조다.

### 한계
- 여전히 **단일 모델, 단일 실행** 관점에 머무름
- 복잡한 멀티스텝 작업에서 오류 복구, 병렬 실행, 재시도 로직을 다루기 어려움
- "컨텍스트를 잘 채우는 것"만으로는 에이전트 시스템의 신뢰성을 보장할 수 없음

---

## 3. 하네스 엔지니어링 (Harness Engineering)

### 개념
AI 에이전트가 **실제 작업을 수행하는 전체 실행 환경(하네스)** 을 설계하는 기술. 모델 자체가 아니라, 모델을 감싸는 시스템 전체를 엔지니어링하는 것.

> "하네스(harness)"는 말에 채우는 마구(馬具)에서 유래. 강력한 존재(LLM)의 힘을 안전하고 방향성 있게 활용하기 위한 제어 구조.

### 핵심 구성요소
```
하네스 = 훅(Hooks)            # 이벤트 기반 사이드이펙트 제어
        + 퍼미션 시스템        # 도구 실행 권한 관리
        + 서브에이전트 조율    # 병렬/순차 에이전트 실행
        + 상태 & 메모리        # 세션 간 영속성
        + 에러 복구 로직       # 실패 감지 및 재시도
        + 옵저버빌리티         # 실행 추적 및 감사
```

### 자율 에이전트의 동작 구조

![Autonomous Agent: LLM이 Environment에 Action을 보내고 Feedback을 받는 루프, Human이 점선으로 감독하며 Stop 조건으로 종료](https://cdn.sanity.io/images/4zrzovbb/website/58d9f10c985c4eb5d53798dea315f7bb5ab6249e-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
>
> 에이전트가 환경(파일시스템, API, 터미널)에 **Action**을 수행하고 **Feedback**을 받아 다음 행동을 결정하는 루프. Human은 점선(필요 시 개입)으로 표시되고 **Stop** 조건이 루프를 종료함. 이 루프 전체를 제어하는 것이 하네스의 역할.

### 구체적 사례 (Claude Code 기준)
- **Hooks**: `PreToolUse`, `PostToolUse` 훅으로 도구 실행 전후 동작 제어
- **Permission Mode**: 자동 승인/거부 정책으로 안전한 실행 경계 설정
- **Sub-agents**: 병렬 에이전트 실행으로 복잡한 작업 분산
- **Skills**: 재사용 가능한 에이전트 행동 패턴 캡슐화
- **CLAUDE.md**: 프로젝트별 에이전트 동작 규칙 정의

### 핵심 관점의 전환
| 관점 | 질문 |
|------|------|
| 프롬프트 엔지니어링 | "어떻게 물어볼까?" |
| 컨텍스트 엔지니어링 | "무엇을 보여줄까?" |
| 하네스 엔지니어링 | "어떻게 실행시킬까?" |

---

## 4. 변화의 흐름과 이유

### 왜 진화했는가?

#### 1단계 → 2단계: 모델 능력의 폭발적 성장
- GPT-3 수준에서는 "잘 물어보는 것"이 결정적 차이를 만들었음
- GPT-4 이후, 모델 자체 추론력이 충분해져 프롬프트 표현보다 **제공되는 정보의 품질과 구성**이 더 중요해짐
- RAG, 툴 호출, 멀티턴 대화가 일반화됨

#### 2단계 → 3단계: 에이전트 시대의 도래
- 단순 질의응답 → 실제 코드 작성, 파일 수정, API 호출 등 **실세계 작업 수행**
- 에이전트가 오랜 시간 자율적으로 동작하면서 **신뢰성, 안전성, 감사 가능성**이 핵심 요구사항으로 등장
- "모델이 좋은 답을 내는 것"보다 "시스템이 올바르게 동작하는 것"이 중요해짐
- 단일 모델 실행 → 멀티에이전트 협업으로 복잡도 증가

### 변화의 본질
```
프롬프트 엔지니어링  →  NLP 문제 (언어 최적화)
컨텍스트 엔지니어링  →  정보 설계 문제 (데이터 큐레이션)
하네스 엔지니어링    →  시스템 엔지니어링 문제 (신뢰성, 안전성, 제어)
```

---

## 5. 중요성과 시사점

### 하네스 엔지니어링이 중요한 이유

#### 안전성 (Safety)
에이전트가 파일을 삭제하거나 외부 API를 호출하는 등 **돌이킬 수 없는 작업**을 수행할 때, 실행 환경의 제어 구조가 사고를 막는 마지막 방어선.

#### 신뢰성 (Reliability)
멀티스텝 작업에서 중간 실패를 감지하고 복구하는 로직은 컨텍스트 설계만으로는 해결 불가. 하네스 수준의 오류 처리 필요.

#### 확장성 (Scalability)
병렬 에이전트 실행, 태스크 분배, 결과 집계는 하네스 레벨에서 설계되어야 함.

#### 감사 가능성 (Auditability)
"에이전트가 무엇을 했는가"를 추적하고 검증할 수 있어야 실제 프로덕션 환경에서 사용 가능.

### 커널 개발자 관점에서의 유사점
| 하네스 엔지니어링 | 커널/시스템 프로그래밍 |
|------------------|----------------------|
| 에이전트 실행 제어 | 프로세스 스케줄링 |
| 훅(Hook) 시스템 | 커널 훅, 인터럽트 핸들러 |
| 퍼미션 시스템 | 파일 권한, capability |
| 상태 관리 | 컨텍스트 스위칭, 상태 저장 |
| 옵저버빌리티 | ftrace, perf, 시스템 로그 |

LLM을 "강력하지만 제어가 필요한 하드웨어"로 보고, 하네스를 그 위의 "운영체제 레이어"로 이해하면 직관적.

---

## 6. 결론

| | 프롬프트 엔지니어링 | 컨텍스트 엔지니어링 | 하네스 엔지니어링 |
|--|--|--|--|
| **주체** | 언어 전문가 | 데이터/정보 설계자 | 시스템 엔지니어 |
| **핵심 스킬** | 글쓰기, 언어학 | 정보 검색, 큐레이션 | 시스템 설계, 신뢰성 공학 |
| **결과물** | 좋은 프롬프트 | 잘 구성된 컨텍스트 | 안정적인 에이전트 시스템 |
| **적용 범위** | 단일 쿼리 | 단일 세션 | 장기 자율 실행 |

> 세 패러다임은 서로를 대체하는 것이 아니라, **누적적으로 쌓이는 레이어**다.
> 좋은 하네스는 잘 설계된 컨텍스트를 포함하고, 잘 설계된 컨텍스트는 좋은 프롬프트를 포함한다.
> 에이전트 시대의 엔지니어는 세 레이어 모두를 이해해야 한다.

---

## 부록. 멀티에이전트 설계 패턴: Planner / Generator / Evaluator

하네스 엔지니어링의 핵심 오케스트레이션 패턴. 복잡한 작업을 단일 에이전트가 처리할 때 발생하는 오류율과 확증 편향을 극복하기 위해 역할을 분리한 구조.

### 이론적 배경 (학술 기원)

| 논문 | 연도 | 기여 |
|------|------|------|
| Chain-of-Thought (Wei et al.) | 2022 | 복잡한 추론을 단계별로 분해 → Planner의 기초 |
| ReAct (Yao et al.) | 2023 | 추론과 행동의 교차 실행, 피드백 루프의 기초 |
| Reflexion (Shinn et al.) | 2023 | 언어 기반 자기 평가 및 기억 → Evaluator의 기초 |
| Self-Refine (Madaan et al.) | 2023 | 생성 → 피드백 → 개선 반복 루프 |
| Constitutional AI (Bai et al.) | 2022 | 외부 레이블 없이 원칙 기반 평가 → Evaluator 일반화 |

---

### Anthropic의 5가지 워크플로우 패턴

Anthropic은 "Building Effective Agents" 에서 에이전트 시스템을 두 가지로 구분한다:
- **Workflows**: LLM이 미리 정해진 경로로 동작 (예측 가능, 신뢰성 높음)
- **Agents**: LLM이 스스로 프로세스와 툴 사용을 동적으로 결정 (유연성 높음, 복잡도 증가)

그리고 5가지 핵심 워크플로우 패턴을 제시한다:

| 패턴 | 설명 | P/G/E 매핑 |
|------|------|-----------|
| Prompt Chaining | 순차적 LLM 호출 체인 | Generator 연속 실행 |
| Routing | 입력 분류 후 전문 경로로 분기 | Planner의 태스크 분배 |
| Parallelization | 독립 서브태스크 동시 실행 | 병렬 Generator |
| **Orchestrator-Workers** | 중앙 LLM이 동적으로 태스크 분해·위임 | **Planner + Generator** |
| **Evaluator-Optimizer** | Generator ↔ Evaluator 반복 루프 | **Generator + Evaluator** |

### 전체 구조 한눈에 보기

#### Orchestrator-Workers: Planner → Generators → Synthesizer 흐름

![Orchestrator-Workers: Orchestrator가 LLM Call 1/2/3을 병렬로 위임하고 Synthesizer가 결과를 취합하여 Out으로 출력](https://cdn.sanity.io/images/4zrzovbb/website/8985fc683fae4780fb34eab1365ab78c7e51bc8e-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
>
> **Orchestrator**(Planner)가 복잡한 태스크를 분해해 **LLM Call 1/2/3**(Generator들)에게 병렬 위임하고, **Synthesizer**가 결과를 통합. "코딩 에이전트처럼 여러 파일을 동시에 수정해야 하는 태스크"에 최적.

#### Evaluator-Optimizer: Generator ↔ Evaluator 피드백 루프

![Evaluator-Optimizer: LLM Call Generator가 Solution을 보내면 LLM Call Evaluator가 Accepted(완료) 또는 Rejected+Feedback(재시도)을 결정](https://cdn.sanity.io/images/4zrzovbb/website/14f51e6406ccb29e695da48b17017e899a6119c7-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
>
> **Generator**가 Solution을 생성하면 **Evaluator**가 평가: Accepted면 Out으로 종료, Rejected+Feedback이면 Generator로 피드백을 보내 반복. "번역처럼 명확한 품질 기준이 있는 태스크"에 적합.

---

### 각 역할의 상세 정의

#### Planner (계획자)

**역할**: 고수준 목표를 받아 구체적이고 실행 가능한 세부 계획으로 분해.

**주요 변형**:

| 유형 | 설명 | 적합한 상황 |
|------|------|------------|
| 선형 분해 | A → B → C 순차 나열 | 단순하고 독립적인 태스크 |
| 계층적 분해 (HTN) | 고수준 전략 → 전술 → 세부 행동 트리 구조 | 로봇공학, 복잡한 의존관계 |
| 그래프 기반 | DAG 형태의 병렬/조건 분기 포함 | 병렬 실행 가능한 태스크 |
| 동적 재계획 | 실행 피드백에 따라 계획 실시간 수정 | 불확실성이 높은 환경 |

#### Generator (생성자)

**역할**: Planner의 계획을 받아 실제 결과물(코드, 텍스트, 액션)을 생성.

**결과물 유형**:
- **코드 생성**: 함수, 모듈, 테스트 코드
- **텍스트 생성**: 문서, 분석, 요약
- **액션 생성**: 툴 호출, 시스템 명령
- **의사결정 생성**: 여러 선택지 중 최적안 선택

**계획을 받는 방식**:
- 명시적 태스크 항목 (태스크 설명 + 제약조건 + 완료 기준)
- 전체 계획을 컨텍스트로 전달 (큰 그림 파악)
- 이전 Generator 출력을 다음 입력으로 연결

#### Evaluator (평가자)

**역할**: Generator 출력이 완료 기준을 충족하는지 검증하고 피드백 생성.

**평가 유형**:

| 유형 | 방식 | 장점 | 단점 |
|------|------|------|------|
| 자기 평가 | 동일 모델이 스스로 평가 | 빠름, 저비용 | 확증 편향 위험 |
| 교차 평가 | 다른 모델 인스턴스가 평가 | 편향 감소 | 추가 비용/지연 |
| 외부 검증 | 코드 컴파일, 테스트 실행 | 객관적 | 프로그래밍 가능한 검증만 |
| 인간 평가 | 사람이 직접 리뷰 | 가장 신뢰성 높음 | 느리고 비쌈 |

**피드백 형태**:
- **이진(Pass/Fail)**: 빠르지만 개선 정보 없음
- **점수(1-10)**: 정량적이지만 방향성 부족
- **언어 피드백**: 구체적 문제점과 개선 방향 포함 → 가장 효과적
- **구조화 피드백**: JSON 형식으로 항목별 평가
- **근거 기반 피드백**: 출력의 특정 부분을 인용하며 비판

---

### 아키텍처 변형

#### 1. 선형 파이프라인 vs 피드백 루프

```
[선형 파이프라인]
Plan → Generate → Evaluate → 완료 (실패 시 사람이 개입)

[피드백 루프]
Plan → Generate → Evaluate
              ↑         |
              └─ FAIL ──┘  (최대 N회 반복)
              → PASS → 완료
```

![Evaluator-Optimizer 피드백 루프: Generator가 Solution을 보내면 Evaluator가 Accepted(종료) 또는 Rejected+Feedback(재시도)을 결정하는 루프](https://cdn.sanity.io/images/4zrzovbb/website/14f51e6406ccb29e695da48b17017e899a6119c7-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
> Generator의 출력(Solution)이 Evaluator로 전달되고, 기준 미달 시 Rejected+Feedback이 다시 Generator로 돌아와 개선을 유도. 기준 충족 시 Accepted로 루프 종료.

| | 선형 파이프라인 | 피드백 루프 |
|--|--|--|
| 속도 | 빠름 | 느림 |
| 비용 | 낮음 (3회 호출) | 높음 (3N회 호출) |
| 품질 | 기본 | 높음 (반복 개선) |
| 적합한 상황 | 단순, 고성공률 태스크 | 복잡, 품질 중요 태스크 |

#### 2. 단일 LLM(역할 프롬프팅) vs 별도 인스턴스

```
[단일 LLM - 역할 프롬프팅]
모델A (Planner 역할) → 모델A (Generator 역할) → 모델A (Evaluator 역할)

[별도 인스턴스]
모델A (Planner 전용) → 모델B (Generator 전용) → 모델C (Evaluator 전용)
```

**병렬 Generator (Parallelization 패턴)**

![Parallelization: In에서 LLM Call 1/2/3이 병렬로 실행되고 Aggregator가 결과를 통합하여 Out으로 출력](https://cdn.sanity.io/images/4zrzovbb/website/406bb032ca007fd1624f261af717d70e6ca86286-2401x1000.png)

> *출처: Anthropic — [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents)*
> 여러 Generator가 동시에 독립적으로 작업하고 **Aggregator**(Evaluator 역할)가 결과를 선택·통합. 다수결 투표(voting)나 다각도 분석에 활용.

- **단일 LLM**: 비용·지연 최소화, 확증 편향 위험
- **별도 인스턴스**: 편향 감소, 전문화 가능, 복잡한 작업에서 15-17배 성능 향상 (단, 구조화된 조율이 없으면 오히려 오류가 17.2배 증폭됨)

#### 3. 주요 구현 패턴 비교

| 패턴 | 설명 | 성능 향상 |
|------|------|----------|
| **Reflexion** | Actor → Evaluator → Self-Reflector 루프 + 에피소딕 메모리 | HumanEval 80% → 91% |
| **Self-Refine** | 단일 모델의 생성 → 자기비평 → 개선 반복 | 인간 선호도 +20% |
| **Constitutional AI** | 원칙 집합으로 평가 → 외부 레이블 없이 개선 | 범용 평가 가능 |

---

### 실제 프레임워크에서의 구현

#### Anthropic 공식 패턴과 프레임워크의 대응

![Orchestrator-Workers: Orchestrator가 LLM Call 1/2/3에게 동적으로 태스크를 위임하고 Synthesizer가 결과를 통합](https://cdn.sanity.io/images/4zrzovbb/website/8985fc683fae4780fb34eab1365ab78c7e51bc8e-2401x1000.png)

> *Planner 역할의 Orchestrator가 태스크를 동적으로 분해해 Worker LLM들(Generator)에게 위임. Claude Code의 Orchestrator → Subagents 구조, LangGraph의 Supervisor → Worker 구조와 동일한 패턴*

#### LangGraph
- **Nodes** = 각 역할(Planner/Generator/Evaluator) 함수
- **Conditional Edges** = 평가 결과에 따라 Generator로 피드백 루프 또는 종료
- **StateGraph** = 계획·실행·평가 결과를 유지하는 중앙 상태

#### AutoGen (Microsoft)
- **ConversableAgent** = 역할별 시스템 프롬프트로 구분
- **Sequential/Group Chat** = 에이전트 간 대화 토폴로지로 협업 구조 정의
- 자연스러운 대화 흐름이지만 종료 조건을 명시해야 루프에 빠지지 않음

#### CrewAI
- **Agent** = 역할(role), 목표(goal), 전문분야(expertise) 명시
- **Process** = Sequential(순차) 또는 Hierarchical(관리자 위임) 프로세스
- 역할 전문화가 명확하지만 동적 흐름은 LangGraph보다 제한적

#### Claude Code
- **Orchestrator** = Planner + 최종 Evaluator 역할
- **Subagents** = Generator 역할 (독립 컨텍스트 윈도우, 병렬 실행 가능)
- 공유 파일시스템으로 상태 공유, 에이전트 간 직접 통신은 없음

| 프레임워크 | 패턴 유형 | 반복 지원 | 가장 적합한 상황 |
|-----------|---------|---------|--------------|
| LangGraph | 그래프 기반 | 유연 | 복잡한 워크플로우, DAG |
| AutoGen | 대화 기반 | 자연스러움 | 동적 멀티에이전트 협업 |
| CrewAI | 역할 기반 | 구조적 | 역할 전문화, 태스크 할당 |
| Claude Code | 계층적 | Orchestrator 통해 | 코드 생성, 서브에이전트 위임 |

---

### 하네스 엔지니어링과의 관계

P/G/E 패턴은 하네스의 **피드백 루프 및 제어 시스템 레이어** 그 자체다.

```
하네스 엔지니어링
├── 컨텍스트 엔지니어링 (정보 구성)
│   └── 프롬프트 엔지니어링 (언어 최적화)
├── [P/G/E 패턴] ← 여기에 위치
│   ├── Planner: 사용자 의도 → 실행 계획
│   ├── Generator: 계획 → 실제 결과물 (툴 실행 포함)
│   └── Evaluator: 결과물 검증 → 피드백 루프
├── 상태 & 메모리 관리
├── 가드레일 & 퍼미션
└── 옵저버빌리티 & 감사
```

Evaluator가 생성하는 피드백 루프가 바로 에이전트를 **자율적으로 동작**하게 만드는 핵심 메커니즘. 이것 없이는 에이전트가 첫 번째 실패에서 멈춰버림.

---

### 실패 유형과 대응 전략

| 실패 유형 | 원인 | 증상 | 대응 |
|----------|------|------|------|
| **확증 편향** | 동일 모델이 자기 출력을 평가 | 반복해도 같은 오류 지속 | 교차 평가 또는 외부 검증 도입 |
| **환각 피드백** | Evaluator가 출력과 무관한 피드백 생성 | 피드백이 실제 오류와 불일치 | 출력 특정 부분 인용 강제 |
| **시간적 고착** | 첫 번째 검색 결과에 과도하게 의존 | 더 나은 접근법을 무시 | 명시적 재계획 또는 다중 탐색 |
| **조율 실패** | 비구조적 멀티에이전트 → 오류 17.2배 증폭 | 에이전트 출력 충돌 | 계층적 조율 (Orchestrator 패턴) |
| **모드 붕괴** | 피드백 후에도 동일 출력 반복 | 반복해도 품질 정체 | 탐색 강제 (다른 접근법 명시) |
| **오류 전파** | 중간 검증 없는 선형 파이프라인 | 작은 오류가 증폭되어 전체 실패 | 단계 간 중간 검증 삽입 |

#### 프로덕션 권장 복구 전략

```
1. Generator 출력
2. Evaluator 검증
   → PASS: 완료
   → FAIL: 언어 피드백 + 재시도 (최대 3회)
3. 3회 모두 실패 시
   → 대체 Evaluator(다른 모델)로 재검증
   → 그래도 실패 시 → Planner가 태스크 재분해
4. 재분해 후에도 실패
   → 사람에게 에스컬레이션 (계획+출력+피드백 시각화)
```

---

### 커널 개발자 관점의 유사점 (확장)

| P/G/E 패턴 | 커널/시스템 프로그래밍 |
|-----------|----------------------|
| Planner | 태스크 스케줄러, 작업 큐 분배 |
| Generator | 워커 스레드, 실제 연산 수행 |
| Evaluator | 검증 루틴, assertion, 테스트 프레임워크 |
| 피드백 루프 | 제어 시스템의 PID 컨트롤러 |
| 최대 재시도 N회 | watchdog 타이머, 재시도 제한 |
| 에스컬레이션 | 커널 패닉 → 사람이 로그 분석 |

> P/G/E 패턴을 "에이전트를 위한 PID 컨트롤러"로 이해하면 직관적.
> Evaluator의 오차 신호(피드백)가 Generator의 다음 출력을 보정하는 구조가 동일함.

---

### 참고 논문 및 자료

**Anthropic 공식 문서**
- [Building Effective Agents — Anthropic](https://www.anthropic.com/research/building-effective-agents) ← **가장 먼저 읽을 것**

**학술 논문**
- [Chain-of-Thought Prompting (Wei et al., 2022)](https://arxiv.org/abs/2201.11903)
- [ReAct: Reasoning + Acting (Yao et al., 2023)](https://arxiv.org/abs/2210.03629)
- [Reflexion: Verbal Reinforcement Learning (Shinn et al., 2023)](https://arxiv.org/abs/2303.11366)
- [Self-Refine: Iterative Refinement (Madaan et al., 2023)](https://arxiv.org/abs/2303.17651)
- [Constitutional AI (Bai et al., 2022)](https://arxiv.org/abs/2212.08073)
- [AutoGen: Multi-Agent Conversation (2023)](https://arxiv.org/abs/2308.08155)

**다이어그램 출처**
- Anthropic CDN 이미지: [Building Effective Agents](https://www.anthropic.com/research/building-effective-agents) (Augmented LLM, Orchestrator-Workers, Evaluator-Optimizer, Parallelization, Autonomous Agent)
- philschmid.de: [Zero to One: Learning Agentic Patterns](https://www.philschmid.de/agentic-pattern)
