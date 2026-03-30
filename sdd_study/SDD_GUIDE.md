# SDD (Specification-Driven Development) 학습 가이드

> **명세 주도 개발** — 코드보다 명세를 먼저, 테스트보다 계약을 먼저

---

## 목차

1. [SDD란 무엇인가?](#1-sdd란-무엇인가)
2. [TDD vs SDD — 무엇이 다른가?](#2-tdd-vs-sdd--무엇이-다른가)
3. [핵심 개념](#3-핵심-개념)
4. [SDD 개발 흐름](#4-sdd-개발-흐름)
5. [Design by Contract (DbC)](#5-design-by-contract-dbc)
6. [임베디드/커널 개발에서의 SDD](#6-임베디드커널-개발에서의-sdd)
7. [SDD 실습 커리큘럼](#7-sdd-실습-커리큘럼)
8. [SDD 도구들](#8-sdd-도구들)
9. [SDD의 장단점](#9-sdd의-장단점)
10. [정리: TDD + SDD 조합](#10-정리-tdd--sdd-조합)

---

## 1. SDD란 무엇인가?

**SDD(Specification-Driven Development, 명세 주도 개발)**는 구현 코드보다 **명세(Specification)**를 먼저 작성하는 소프트웨어 개발 방법론이다.

```
일반 개발:    [머릿속 설계] → [코드 작성] → [테스트]
TDD:          [테스트 작성] → [코드 작성] → [리팩터링]
SDD:          [명세 작성]  → [테스트 작성] → [코드 작성] → [검증]
```

### SDD의 핵심 철학

> "무엇을 만들 것인가를 완전히 정의한 뒤에 어떻게 만들 것인가를 고민한다."

명세는 단순한 주석이나 문서가 아니다. **기계가 읽고 검증할 수 있는 형식적 계약(Formal Contract)**이다.

| 구분 | 내용 |
|------|------|
| 명세의 대상 | 함수의 입력, 출력, 부수효과, 상태 변화 |
| 명세의 형식 | 사전조건, 사후조건, 불변식 |
| 명세의 목적 | 구현 전 완전한 행동 정의 + 자동 검증 |

---

## 2. TDD vs SDD — 무엇이 다른가?

### 비교표

| 항목 | TDD | SDD |
|------|-----|-----|
| 시작점 | 실패하는 테스트 | 형식적 명세(계약) |
| 주된 산출물 | 테스트 코드 | 명세 문서 + 계약 코드 |
| 검증 방식 | 단위 테스트 | 속성 기반 테스트 + 정형 검증 |
| 추상화 수준 | 구체적 예제 중심 | 일반적 속성 중심 |
| 적합한 상황 | 비즈니스 로직, 빠른 피드백 | 안전-critical 시스템, 프로토콜, API |
| 학습 난이도 | 낮음 | 높음 |

### 예시: 정수 나눗셈 함수

**TDD 방식** — 구체적 예제로 검증:
```c
// 테스트가 명세를 대신함
assert(divide(10, 2) == 5);
assert(divide(7, 2) == 3);   // 정수 나눗셈
// 그런데 divide(5, 0)은? → 테스트를 빠뜨리기 쉬움
```

**SDD 방식** — 속성으로 완전히 명세:
```c
/*
 * @spec divide(a, b)
 * @pre  b != 0                          // 사전조건
 * @post result == a / b                 // 사후조건 (C 정수 나눗셈)
 * @post (result * b + a % b) == a       // 항등식 불변 유지
 * @pure                                 // 부수효과 없음
 */
int divide(int a, int b);
```

→ 명세에서 `b != 0` 사전조건이 명시되어 있으므로, 호출자는 0을 넘기면 안 된다는 **계약**이 성립한다.

---

## 3. 핵심 개념

### 3.1 사전조건 (Precondition)

함수 호출 **전**에 반드시 참이어야 하는 조건. **호출자(caller)의 의무**.

```c
/*
 * @pre buf != NULL
 * @pre size > 0
 * @pre size <= MAX_BUF_SIZE
 */
int ring_buffer_init(ring_buffer_t *buf, size_t size);
```

사전조건이 위반되면 → **호출자의 버그**. 함수는 이 상황에 대해 책임지지 않는다.

### 3.2 사후조건 (Postcondition)

함수 실행 **후**에 반드시 참이어야 하는 조건. **구현자(callee)의 의무**.

```c
/*
 * @pre  !ring_buffer_full(buf)
 * @post ring_buffer_size(buf) == \old(ring_buffer_size(buf)) + 1
 * @post ring_buffer_peek_back(buf) == data
 */
void ring_buffer_push(ring_buffer_t *buf, uint8_t data);
```

사후조건이 위반되면 → **구현자의 버그**.

### 3.3 불변식 (Invariant)

객체/모듈이 **항상** 유지해야 하는 조건. 모든 public 함수 호출 전후에 성립해야 한다.

```c
/*
 * ring_buffer 불변식:
 * - 0 <= size <= capacity
 * - head < capacity
 * - tail < capacity
 * - size == (tail - head + capacity) % capacity  (또는 size == 0일 때)
 */
```

### 3.4 순수성 (Purity) / 부수효과 (Side Effect)

```c
// @pure: 외부 상태 변경 없음, 같은 입력 → 항상 같은 출력
int calculate(int a, int b);

// @modifies buf->data, buf->head: 이 필드만 변경함
void ring_buffer_push(ring_buffer_t *buf, uint8_t data);
```

### 3.5 속성 기반 테스트 (Property-Based Testing)

SDD에서 명세한 속성을 자동으로 수천 가지 입력으로 검증:

```c
// TDD: 구체적 케이스
assert(sort([3,1,2]) == [1,2,3]);

// SDD 속성 기반 테스트:
// 속성 1: 정렬 후 길이는 같다
// 속성 2: 정렬 후 인접 요소는 arr[i] <= arr[i+1]
// 속성 3: 정렬 전후 원소 집합이 같다 (같은 원소, 같은 빈도)
// → 임의의 배열에 대해 위 세 속성을 모두 검증
```

---

## 4. SDD 개발 흐름

```
┌─────────────────────────────────────────────────────────────┐
│                    SDD 개발 사이클                           │
└─────────────────────────────────────────────────────────────┘

  1. 요구사항 분석
       ↓
  2. 인터페이스 설계 (헤더/API 정의)
       ↓
  3. 형식적 명세 작성
     • 사전조건 (@pre)
     • 사후조건 (@post)
     • 불변식 (@invariant)
     • 부수효과 (@modifies)
       ↓
  4. 명세 검토 (동료 리뷰 / 정형 검증 도구)
       ↓
  5. 명세 기반 테스트 작성
     • 속성 기반 테스트 (Property-Based Test)
     • 경계값 테스트
     • 사전조건 위반 테스트
       ↓
  6. 구현
       ↓
  7. 런타임 계약 검증 (assert, 계약 라이브러리)
       ↓
  8. 정적 분석 / 정형 검증 (Frama-C, CBMC 등)
```

---

## 5. Design by Contract (DbC)

SDD의 핵심 메커니즘. Bertrand Meyer가 Eiffel 언어에서 처음 도입.

### 5.1 계약의 비유

```
서비스 계약서와 동일:
- 고객(호출자)의 의무: 사전조건 충족
- 서비스(함수)의 의무: 사후조건 + 불변식 보장
- 어느 쪽이 위반하면 → 해당 쪽의 버그
```

### 5.2 C에서 DbC 구현

C에는 언어 차원의 계약 문법이 없으므로, 직접 구현한다:

```c
/* contract.h — 간단한 DbC 매크로 */
#ifdef NDEBUG
  #define REQUIRE(cond)    ((void)0)   /* 사전조건: 릴리즈에서 제거 */
  #define ENSURE(cond)     ((void)0)   /* 사후조건: 릴리즈에서 제거 */
  #define INVARIANT(cond)  ((void)0)
#else
  #define REQUIRE(cond) \
    do { if (!(cond)) { \
      fprintf(stderr, "Precondition failed: %s\n  at %s:%d\n", \
              #cond, __FILE__, __LINE__); \
      abort(); \
    }} while(0)

  #define ENSURE(cond) \
    do { if (!(cond)) { \
      fprintf(stderr, "Postcondition failed: %s\n  at %s:%d\n", \
              #cond, __FILE__, __LINE__); \
      abort(); \
    }} while(0)

  #define INVARIANT(cond) \
    do { if (!(cond)) { \
      fprintf(stderr, "Invariant violated: %s\n  at %s:%d\n", \
              #cond, __FILE__, __LINE__); \
      abort(); \
    }} while(0)
#endif
```

### 5.3 DbC 적용 예시

```c
/* ring_buffer.h */

/**
 * @brief 링 버퍼에 데이터 추가
 *
 * @pre  buf != NULL
 * @pre  !ring_buffer_full(buf)
 * @post ring_buffer_size(buf) == OLD_SIZE + 1
 * @post ring_buffer_peek_back(buf) == data
 * @modifies buf->data[buf->tail], buf->tail, buf->size
 */
void ring_buffer_push(ring_buffer_t *buf, uint8_t data);
```

```c
/* ring_buffer.c */
void ring_buffer_push(ring_buffer_t *buf, uint8_t data)
{
    /* 사전조건 검사 */
    REQUIRE(buf != NULL);
    REQUIRE(!ring_buffer_full(buf));

    size_t old_size = buf->size;  /* 사후조건 검증용 스냅샷 */

    /* 구현 */
    buf->data[buf->tail] = data;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->size++;

    /* 사후조건 검사 */
    ENSURE(buf->size == old_size + 1);
    ENSURE(buf->data[(buf->tail - 1 + buf->capacity) % buf->capacity] == data);

    /* 불변식 검사 */
    INVARIANT(buf->size <= buf->capacity);
}
```

---

## 6. 임베디드/커널 개발에서의 SDD

SDD는 특히 임베디드/커널 개발에서 강력하다.

### 6.1 왜 임베디드에서 SDD가 중요한가?

```
임베디드 시스템의 특성:
├── 디버깅 어려움 (하드웨어 의존적)
├── 런타임 오류 → 시스템 크래시 (재부팅 비용 큼)
├── 안전-critical (자동차, 의료, 항공)
└── 복잡한 하드웨어 인터페이스 (레지스터, 인터럽트, DMA)

SDD의 해결책:
├── 명세가 코드보다 먼저 → 하드웨어 없이 설계 검증 가능
├── 정형 검증 → 컴파일 타임/분석 타임에 버그 발견
├── 계약 기반 → 계층 간 인터페이스 명확화
└── 속성 기반 테스트 → 에지케이스 자동 탐색
```

### 6.2 Linux 커널에서의 명세적 사고

Linux 커널은 공식 SDD 도구를 쓰진 않지만, 암묵적 계약이 존재한다:

```c
/**
 * mutex_lock - 뮤텍스 획득
 * @lock: 획득할 뮤텍스
 *
 * @pre  !in_interrupt()          /* 인터럽트 컨텍스트에서 호출 금지 */
 * @pre  !in_atomic()             /* atomic 컨텍스트에서 호출 금지 */
 * @post mutex_is_locked(lock)    /* 호출 후 뮤텍스는 잠김 상태 */
 * @modifies lock->owner, lock->wait_list
 */
void mutex_lock(struct mutex *lock);
```

`might_sleep()` 매크로가 바로 사전조건 검사다:
```c
void mutex_lock(struct mutex *lock)
{
    might_sleep();   /* @pre: 슬립 가능한 컨텍스트여야 함 */
    /* ... */
}
```

### 6.3 드라이버 인터페이스 명세 예시

```c
/**
 * @brief GPIO 드라이버 초기화
 *
 * @pre  gpio_num < GPIO_MAX_NUM
 * @pre  direction ∈ {GPIO_IN, GPIO_OUT}
 * @pre  해당 GPIO가 이미 사용 중이 아님
 *
 * @post gpio_is_initialized(gpio_num) == true
 * @post gpio_get_direction(gpio_num) == direction
 *
 * @return 0: 성공
 *         -EINVAL: 잘못된 GPIO 번호 또는 방향
 *         -EBUSY: 이미 사용 중인 GPIO
 *
 * @modifies gpio_table[gpio_num]
 */
int gpio_init(uint8_t gpio_num, gpio_direction_t direction);
```

---

## 7. SDD 실습 커리큘럼

### 모듈 01: 계약 기반 스택 (Contract-Based Stack)

TDD로 이미 만든 모듈을 SDD로 재설계한다.

**목표**: 명세 → 계약 코드 → 속성 기반 테스트의 흐름 체험

```
01_stack/
├── spec.md          ← 형식적 명세 문서
├── stack.h          ← 계약 주석 포함한 인터페이스
├── stack.c          ← REQUIRE/ENSURE 포함한 구현
├── contract.h       ← DbC 매크로
├── test_stack.c     ← 속성 기반 테스트
└── Makefile
```

### 모듈 02: 프로토콜 명세 (Protocol Specification)

통신 프로토콜의 상태 전이를 명세로 정의.

```
02_protocol/
├── spec.md          ← 프로토콜 상태 기계 명세
├── protocol.h
├── protocol.c
└── test_protocol.c  ← 상태 전이 속성 테스트
```

### 모듈 03: 메모리 안전 명세 (Memory Safety)

메모리 풀의 안전 속성을 명세하고 검증.

```
03_mem_safe/
├── spec.md          ← 메모리 안전 속성 명세
├── mem_pool.h
├── mem_pool.c
└── test_properties.c
```

---

## 8. SDD 도구들

### C/임베디드 환경

| 도구 | 역할 | 특징 |
|------|------|------|
| **Frama-C** | 정형 검증 | ACSL 명세 언어, 사전/사후조건 자동 증명 |
| **CBMC** | 모델 검사 | 경계값 위반, 버퍼 오버플로우 자동 탐지 |
| **assert.h** | 런타임 계약 | 단순하지만 어디서나 사용 가능 |
| **Csmith** | 속성 기반 | C 코드 자동 생성으로 컴파일러 검증 |

### ACSL (ANSI/ISO C Specification Language)

Frama-C에서 사용하는 C용 공식 명세 언어:

```c
/*@
  requires \valid(buf);
  requires size > 0 && size <= MAX_SIZE;
  ensures \result == 0 || \result == -EINVAL;
  ensures \result == 0 ==> buf->capacity == size;
  assigns *buf;
*/
int ring_buffer_init(ring_buffer_t *buf, size_t size);
```

### 속성 기반 테스트 라이브러리

| 언어 | 라이브러리 | 특징 |
|------|-----------|------|
| C | **theft** | C용 속성 기반 테스트 프레임워크 |
| Python | **Hypothesis** | 가장 성숙한 PBT 도구 |
| Rust | **proptest** | 러스트 생태계 표준 |
| Haskell | **QuickCheck** | PBT의 원조 |

---

## 9. SDD의 장단점

### 장점

```
✅ 설계 품질 향상
   - 인터페이스를 구현 전에 완전히 정의
   - 엣지케이스를 명세 단계에서 발견

✅ 디버깅 비용 절감
   - 계약 위반 시 정확한 위치와 원인 즉시 파악
   - "호출자 버그 vs 구현자 버그" 명확 구분

✅ 안전-critical 시스템 적합
   - 정형 검증 도구와 연계 가능
   - DO-178C, IEC 61508 등 안전 표준 요구사항 충족

✅ 문서화 효과
   - 명세가 곧 살아있는 문서
   - API 사용자에게 정확한 계약 제공
```

### 단점

```
❌ 학습 곡선
   - 형식적 명세 작성법 습득 필요
   - 논리/수학적 사고 요구

❌ 초기 투자 비용
   - TDD보다 명세 작성에 시간이 더 걸림
   - 빠른 프로토타이핑에는 오버엔지니어링

❌ 도구 성숙도
   - C 환경의 SDD 도구는 상대적으로 진입장벽 높음
   - Frama-C 등은 학습 비용이 상당함
```

---

## 10. 정리: TDD + SDD 조합

TDD와 SDD는 경쟁 관계가 아니라 **상호 보완 관계**다.

```
                SDD                    TDD
              ┌────────┐            ┌────────┐
              │ 명세   │            │ 테스트 │
              │ 계약   │ ─────────► │ 작성   │
              │ 작성   │   명세가    │        │
              └────────┘  테스트를  └────────┘
                          안내함         │
                                        ▼
                                   ┌────────┐
                                   │ 구현   │
                                   └────────┘
                                        │
                               ◄────────┘
                          계약 검증
```

### 실무 권장 워크플로우

```
1. [SDD] 인터페이스 명세 작성 (헤더 + 계약 주석)
2. [SDD] 핵심 속성/불변식 정의
3. [TDD] 명세 기반으로 테스트 작성 (구체적 예제)
4. [SDD] 속성 기반 테스트 추가
5. [TDD] Red-Green-Refactor 사이클로 구현
6. [SDD] 런타임 계약(REQUIRE/ENSURE) 추가
7. [SDD] 정적 분석 도구로 명세 검증
```

---

## 참고자료

- **Design by Contract** — Bertrand Meyer, "Object-Oriented Software Construction" (1988)
- **ACSL Reference Manual** — Frama-C 공식 문서
- **"Hoare Logic"** — Tony Hoare, "An Axiomatic Basis for Computer Programming" (1969)
- **QuickCheck** — Koen Claessen & John Hughes, "QuickCheck: A Lightweight Tool for Random Testing of Haskell Programs" (2000)
- **Linux Kernel Documentation** — `Documentation/process/coding-style.rst`

---

*다음 단계: `01_stack/` 모듈부터 시작하여 SDD를 직접 실습해보자.*
