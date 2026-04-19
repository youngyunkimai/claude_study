# Gemma3 챗봇

> 작성일: 2026-04-18

## 목차
- [1. 소개](#1-소개)
- [2. 시스템 요구사항](#2-시스템-요구사항)
- [3. 프로젝트 구조](#3-프로젝트-구조)
- [4. 설치 및 실행](#4-설치-및-실행)
- [5. 기능 설명](#5-기능-설명)
- [6. API 명세](#6-api-명세)
- [7. 지식 저장 구조](#7-지식-저장-구조)
- [참고자료](#참고자료)

---

## 1. 소개

Raspberry Pi 4 위에서 동작하는 로컬 AI 챗봇입니다.
[Ollama](https://ollama.com)로 구동되는 **Gemma3:1b** 모델을 백엔드로 사용하고,
**FastAPI + Vanilla HTML/JS**로 웹 UI를 제공합니다.

대화를 나눌수록 챗봇이 사용자에 대한 사실을 학습합니다. 대화 종료 시 핵심 내용을
JSON 파일로 저장하고, 이후 대화에서 관련 정보를 자동으로 참조합니다.

```
[브라우저] ←──스트리밍──→ [FastAPI 서버] ←──→ [Ollama / Gemma3:1b]
                                  ↕
                          [knowledge.json]
                          (지식 저장/검색)
```

---

## 2. 시스템 요구사항

| 항목 | 최소 사양 |
|------|----------|
| 기기 | Raspberry Pi 4 (4GB RAM) |
| OS | Linux aarch64 |
| Python | 3.10 이상 |
| Ollama | 최신 버전 |
| 여유 디스크 | 2GB 이상 (모델 파일 포함) |

Ollama 설치:
```bash
curl -fsSL https://ollama.com/install.sh | sh
```

---

## 3. 프로젝트 구조

```
chatbot/
├── main.py              # FastAPI 서버, API 라우터
├── knowledge.py         # 지식 저장/검색 (JSON + 키워드)
├── extractor.py         # LLM으로 대화에서 사실 추출
├── requirements.txt     # Python 의존성
├── .venv/               # Python 가상환경 (자동 생성)
├── data/
│   └── knowledge.json   # 누적 지식 저장소 (자동 생성)
└── static/
    ├── index.html       # 채팅 웹 UI
    └── app.js           # 스트리밍 처리, API 호출
```

---

## 4. 설치 및 실행

### 1단계: 모델 다운로드 (최초 1회)

```bash
ollama pull gemma3:1b
```

### 2단계: 가상환경 생성 및 의존성 설치

Debian 계열 OS(Raspberry Pi OS 포함)는 시스템 Python이 외부 관리 환경이므로
가상환경을 생성해야 합니다.

```bash
cd chatbot
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
```

### 3단계: 서버 실행

```bash
.venv/bin/uvicorn main:app --host 0.0.0.0 --port 8000
```

### 4단계: 브라우저 접속

- 로컬: `http://localhost:8000`
- 같은 네트워크의 다른 기기: `http://<RPi4-IP주소>:8000`

---

## 5. 기능 설명

### 채팅

- 메시지를 입력하고 **전송** 버튼 또는 `Enter` 키를 누릅니다.
- Gemma3의 응답이 **스트리밍** 방식으로 실시간 출력됩니다.
- 메모리 절약을 위해 최근 **10턴**의 대화 히스토리만 유지합니다.

### 지식 주입

- 사용자의 메시지 키워드를 기반으로 저장된 지식에서 관련 사실을 자동 검색합니다.
- 관련 사실이 있으면 LLM의 시스템 프롬프트에 추가되어 문맥을 반영한 답변이 나옵니다.

### 지식 저장

- 대화 후 **[종료]** 버튼을 클릭하면 LLM이 대화 내용을 분석해 핵심 사실을 추출합니다.
- 추출된 사실은 중복을 제거한 후 `data/knowledge.json`에 누적 저장됩니다.
- 헤더에 현재 저장된 지식 개수가 실시간으로 표시됩니다.

---

## 6. API 명세

### `POST /chat`

채팅 메시지를 전송하고 스트리밍 응답을 받습니다.

**요청:**
```json
{
  "message": "안녕하세요!",
  "history": [
    {"role": "user", "content": "..."},
    {"role": "assistant", "content": "..."}
  ]
}
```

**응답:** `text/plain` 스트리밍 (토큰 단위)

---

### `POST /extract`

대화 히스토리에서 사실을 추출하고 저장합니다.

**요청:**
```json
{
  "history": [
    {"role": "user", "content": "..."},
    {"role": "assistant", "content": "..."}
  ]
}
```

**응답:**
```json
{
  "extracted": ["사용자의 이름은 철수이다"],
  "added": 1
}
```

---

### `GET /knowledge`

저장된 지식 목록 전체를 조회합니다.

**응답:**
```json
{
  "facts": [
    {
      "id": 1,
      "content": "사용자의 이름은 철수이다",
      "keywords": ["사용자", "이름", "철수"],
      "created_at": "2026-04-18T10:00:00"
    }
  ]
}
```

---

## 7. 지식 저장 구조

지식은 `data/knowledge.json`에 다음 형식으로 저장됩니다.

```json
{
  "facts": [
    {
      "id": 1,
      "content": "사용자는 Python을 5년째 사용하고 있다",
      "keywords": ["사용자", "python", "5년째", "사용하고"],
      "created_at": "2026-04-18T10:00:00"
    }
  ]
}
```

| 필드 | 설명 |
|------|------|
| `id` | 사실 고유 번호 (자동 증가) |
| `content` | 추출된 사실 텍스트 |
| `keywords` | 검색용 키워드 목록 (저장 시 자동 추출) |
| `created_at` | 저장 시각 (ISO 8601) |

---

## 참고자료

- [Ollama 공식 사이트](https://ollama.com) — 로컬 LLM 실행 도구
- [Gemma3 모델 (Ollama)](https://ollama.com/library/gemma3) — Google Gemma3 모델 허브
- [FastAPI 공식 문서](https://fastapi.tiangolo.com) — Python 웹 프레임워크
- [Ollama Python 라이브러리](https://github.com/ollama/ollama-python) — Python에서 Ollama 사용
