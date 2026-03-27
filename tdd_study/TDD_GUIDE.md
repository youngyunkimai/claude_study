# TDD (Test-Driven Development) 학습 가이드

> C 언어로 배우는 TDD — Linux 커널 개발자를 위한 실습 가이드

---

## 목차

1. [TDD란 무엇인가](#1-tdd란-무엇인가)
2. [TDD의 3가지 법칙](#2-tdd의-3가지-법칙)
3. [Red-Green-Refactor 사이클](#3-red-green-refactor-사이클)
4. [FIRST 원칙 — 좋은 테스트의 조건](#4-first-원칙--좋은-테스트의-조건)
5. [테스트 이름 규칙](#5-테스트-이름-규칙)
6. [테스트 프레임워크 사용법](#6-테스트-프레임워크-사용법)
7. [실습 모듈 안내](#7-실습-모듈-안내)
8. [TDD 핵심 스킬 정리](#8-tdd-핵심-스킬-정리)
9. [자주 하는 실수](#9-자주-하는-실수)

---

## 1. TDD란 무엇인가

TDD는 **테스트를 먼저 작성하고, 그 테스트를 통과하는 코드를 그 다음에 작성하는** 개발 방법론입니다.

### 일반적인 개발 방식 vs TDD

```
일반적인 방식:
  코드 작성 → 테스트 작성 → 버그 발견 → 수정

TDD 방식:
  테스트 작성 → 코드 작성 → 리팩토링 → 다음 테스트 작성 → ...
```

### TDD를 배워야 하는 이유

TDD는 단순히 "테스트를 먼저 쓰는 것"이 아닙니다.
**테스트가 설계를 이끄는(Test-Driven Design)** 방법론입니다.

테스트를 먼저 작성하면 자연스럽게:

- **사용하기 좋은 API**가 만들어집니다 (테스트에서 먼저 사용하니까)
- **작은 단위**로 쪼개게 됩니다 (테스트하기 어려운 코드는 설계가 나쁜 코드)
- **명확한 명세**가 생깁니다 (테스트가 곧 문서)
- **리팩토링을 두려워하지 않게** 됩니다 (테스트가 안전망)

### 커널 개발자에게 TDD가 유용한 이유

커널 코드는 특성상 테스트하기 어렵습니다:
- 하드웨어 의존성
- 특권 모드 실행
- 복잡한 동시성

TDD는 이 문제를 해결하는 설계 패턴을 강제합니다:
- **의존성 분리** (ops 구조체, 함수 포인터)
- **명확한 인터페이스**
- **테스트 가능한 단위로 분리**

실제로 커널의 `kunit` 프레임워크, `kselftest`도 같은 철학을 따릅니다.

---

## 2. TDD의 3가지 법칙

Uncle Bob Martin이 정의한 TDD의 세 가지 법칙입니다.
이 법칙을 엄격히 따르면 TDD의 리듬이 몸에 배게 됩니다.

---

### Law 1: 실패하는 테스트 없이 프로덕션 코드를 작성하지 말라

> *"Do not write production code unless it is to make a failing test pass."*

테스트가 없는 코드는 요구사항이 없는 코드입니다.
새 기능을 추가하기 전에 반드시 그 기능을 검증하는 테스트를 먼저 작성하세요.

```c
/* 잘못된 방식: 테스트 없이 구현부터 */
int add(int a, int b) { return a + b; }

/* 올바른 방식: 테스트 먼저 */
void test_add_two_positives_returns_sum(void) {
    TEST_ASSERT_EQUAL_INT(3, add(1, 2));   // add()가 아직 없다! → RED
}
// 이제 add()를 구현
int add(int a, int b) { return a + b; }   // → GREEN
```

---

### Law 2: 실패를 증명하기에 충분한 만큼만 테스트를 작성하라

> *"Write only enough test code to fail. Compilation failure counts as failing."*

한 번에 테스트를 많이 작성하지 마세요.
컴파일 에러도 "실패"입니다. 에러 하나를 확인했으면 멈추고 코드를 수정하세요.

```c
/* 잘못된 방식: 한 번에 10개 테스트 작성 후 구현 */

/* 올바른 방식: 테스트 하나 → RED 확인 → GREEN → 다음 테스트 */
void test_add_two_positives_returns_sum(void) {
    TEST_ASSERT_EQUAL_INT(3, add(1, 2));
}
// make test → RED → 구현 → GREEN → 다음 테스트로
```

---

### Law 3: 실패하는 테스트를 통과하기에 충분한 만큼만 코드를 작성하라

> *"Write only enough production code to make the failing test pass."*

테스트에 없는 기능을 미리 구현하지 마세요.
"나중에 필요할 것 같은" 코드는 TDD의 적입니다.

```c
/* 잘못된 방식: 테스트에 없는 기능까지 미리 구현 */
int add(int a, int b) {
    if (a > INT_MAX - b) return INT_MAX;  // overflow 처리 (테스트에 없음)
    return a + b;
}

/* 올바른 방식: 테스트를 통과할 만큼만 */
int add(int a, int b) {
    return a + b;  // 테스트가 요구하는 것만
}
// overflow 처리가 필요하면 → 먼저 overflow 테스트를 작성
```

---

## 3. Red-Green-Refactor 사이클

TDD의 핵심 리듬입니다. 모든 코드는 이 사이클로 작성됩니다.

```
      ┌─────────────────────────────────────────┐
      │                                         │
      ▼                                         │
   [RED]                                        │
   실패하는 테스트 작성                           │
   컴파일 에러도 RED                             │
      │                                         │
      ▼                                         │
   [GREEN]                                      │
   테스트를 통과할 최소한의 코드 작성             │
   "더러운 코드"도 괜찮음 — 일단 GREEN           │
      │                                         │
      ▼                                         │
   [REFACTOR]                                   │
   코드 구조 개선 (테스트는 여전히 GREEN)         │
   중복 제거, 이름 개선, 패턴 통일               │
      │                                         │
      └─────────────────────────────────────────┘
                  다음 테스트로
```

### 각 단계의 목표

| 단계 | 목표 | 허용되는 것 | 금지되는 것 |
|------|------|-------------|-------------|
| **RED** | 실패를 증명 | 컴파일 에러 | 구현 코드 작성 |
| **GREEN** | 빠르게 통과 | 하드코딩, 더러운 코드 | 기능 추가 |
| **REFACTOR** | 구조 개선 | 내부 변경 | 새 기능, 테스트 변경 |

### 실습 예시 (Calculator Module 1)

```bash
# RED: 테스트 작성, 컴파일 에러 확인
$ make test
fatal error: calculator.h: No such file or directory   ← RED

# calculator.h 생성 (선언만)
$ make test
undefined reference to 'add'                           ← 여전히 RED (링크 에러)

# calculator.c 스텁 작성
int add(int a, int b) { return 0; }
$ make test
FAIL: expected 3, got 0                                ← 여전히 RED (런타임)

# GREEN: 최소 구현
int add(int a, int b) { return a + b; }
$ make test
PASS                                                   ← GREEN!

# REFACTOR: (이 경우 개선할 것 없음, 다음 테스트로)
```

---

## 4. FIRST 원칙 — 좋은 테스트의 조건

좋은 테스트는 FIRST 원칙을 따릅니다.

### F — Fast (빠름)

테스트는 밀리초 안에 실행되어야 합니다.
느린 테스트는 실행을 미루게 만들고, TDD 리듬을 깨뜨립니다.

```c
/* 나쁜 예: 실제 하드웨어 대기 */
void test_led_blink(void) {
    led_set(LED_BLINK);
    sleep(1);                    // 1초 대기 → 느림!
    TEST_ASSERT(...);
}

/* 좋은 예: 가짜 delay로 즉시 실행 */
void fake_delay_ms(int ms) { (void)ms; }  // 실제로 기다리지 않음

void test_led_blink(void) {
    led_fsm_init(&fake_ops, 1);  // fake_ops에 fake_delay_ms 사용
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);  // 즉시 확인
}
```

### I — Independent (독립적)

테스트는 실행 순서에 무관하게 동일한 결과를 내야 합니다.
테스트 간에 상태를 공유하지 마세요.

```c
/* 나쁜 예: 전역 버퍼를 테스트 간에 공유 */
ring_buffer_t g_rb;  // 전역!

void test_push(void) { rb_push(&g_rb, 1); }          // 상태 남김
void test_pop(void)  { rb_pop(&g_rb, &val); }         // 이전 테스트에 의존

/* 좋은 예: 각 테스트가 자체 상태 소유 */
void test_push(void) {
    ring_buffer_t rb;        // 로컬 선언
    rb_init(&rb, 4);
    rb_push(&rb, 1);
    TEST_ASSERT_EQUAL_SIZE(1, rb_count(&rb));
    rb_destroy(&rb);
}
```

### R — Repeatable (반복 가능)

어떤 환경에서도, 몇 번을 실행해도 같은 결과가 나와야 합니다.
외부 상태(파일, 네트워크, 시간)에 의존하지 마세요.

### S — Self-validating (자가 검증)

테스트는 PASS/FAIL을 스스로 판단해야 합니다.
사람이 출력을 보고 "맞는 것 같네"라고 판단하면 안 됩니다.

```c
/* 나쁜 예: 사람이 판단해야 함 */
void test_add(void) {
    printf("result: %d\n", add(1, 2));   // 3이 맞는지 사람이 확인?
}

/* 좋은 예: 자동으로 판단 */
void test_add(void) {
    TEST_ASSERT_EQUAL_INT(3, add(1, 2));   // PASS or FAIL 자동 판단
}
```

### T — Timely (적시에)

테스트는 프로덕션 코드를 작성하기 **직전**에 작성해야 합니다.
코드를 다 작성하고 나서 테스트를 작성하면 TDD가 아닙니다.

---

## 5. 테스트 이름 규칙

테스트 이름은 "무엇을, 어떤 조건에서, 어떻게 동작하는지"를 설명해야 합니다.

### 패턴: `test_[단위]_[시나리오]_[기대결과]`

```c
/* 나쁜 이름: 무엇을 테스트하는지 불분명 */
void test_add(void) { ... }
void test1(void)    { ... }

/* 좋은 이름: 읽기만 해도 명세가 이해됨 */
void test_add_two_positives_returns_sum(void)        { ... }
void test_add_zero_returns_same_number(void)          { ... }
void test_rb_push_when_full_returns_error(void)       { ... }
void test_pool_alloc_when_exhausted_returns_null(void){ ... }
void test_fsm_tick_in_blink_toggles_gpio(void)        { ... }
```

테스트 이름을 읽으면 테스트 코드를 보지 않아도 **명세**가 이해되어야 합니다.
실패했을 때 `[RUN] test_rb_push_when_full_returns_error ... FAIL`만 봐도 어디가 문제인지 알 수 있습니다.

---

## 6. 테스트 프레임워크 사용법

이 프로젝트는 외부 의존성 없는 단일 헤더 프레임워크를 사용합니다.
(`framework/test_framework.h`)

### 어서션 매크로

```c
/* 불리언 조건 검증 */
TEST_ASSERT(rb_is_empty(&rb));
TEST_ASSERT(!rb_is_full(&rb));

/* 정수 동등성 검증 (expected, actual 순서) */
TEST_ASSERT_EQUAL_INT(3, add(1, 2));          // expected=3, actual=add(1,2)
TEST_ASSERT_EQUAL_INT(CALC_OK, err);

/* 포인터 검증 */
TEST_ASSERT_NULL(ptr);                         // ptr == NULL
TEST_ASSERT_NOT_NULL(ptr);                     // ptr != NULL

/* size_t 검증 */
TEST_ASSERT_EQUAL_SIZE(4, rb_count(&rb));

/* 문자열 검증 */
TEST_ASSERT_EQUAL_STR("hello", actual_str);
```

### 테스트 파일 구조

```c
#include "../framework/test_framework.h"
#include "my_module.h"

/* 테스트 함수 */
void test_something_does_expected(void)
{
    /* Arrange: 테스트 조건 설정 */
    my_struct_t s;
    my_init(&s);

    /* Act: 테스트 대상 동작 실행 */
    int result = my_function(&s, 42);

    /* Assert: 결과 검증 */
    TEST_ASSERT_EQUAL_INT(expected, result);
}

int main(void)
{
    RUN_TEST(test_something_does_expected);
    /* ... 더 많은 테스트 ... */

    TEST_SUMMARY();
    return TEST_EXIT_CODE;   /* 실패 시 exit(1) → make도 실패 */
}
```

### Arrange-Act-Assert 패턴

모든 테스트는 세 부분으로 구성됩니다:

```c
void test_rb_pop_returns_pushed_value(void)
{
    /* Arrange: 테스트에 필요한 상태 준비 */
    ring_buffer_t rb;
    rb_init(&rb, 4);
    rb_push(&rb, 0x42);

    /* Act: 테스트하려는 동작 한 가지만 실행 */
    uint8_t val = 0;
    int ret = rb_pop(&rb, &val);

    /* Assert: 기대 결과 검증 */
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0x42, val);
    TEST_ASSERT(rb_is_empty(&rb));

    rb_destroy(&rb);
}
```

---

## 7. 실습 모듈 안내

### 실습 전 준비

```bash
# 빌드 도구 확인
gcc --version
make --version

# 프로젝트 구조 확인
ls tdd_study/
```

### 권장 실습 순서 (각 모듈)

```
1. 테스트 파일을 먼저 읽는다   → 무엇을 구현해야 하는지 이해
2. make test                   → RED 상태 확인 (컴파일/링크 에러)
3. 헤더 파일(스텁) 작성        → make test → 링크/런타임 에러 (여전히 RED)
4. 구현 파일에 최소 코드 추가  → make test → GREEN
5. Refactor                    → make test → 여전히 GREEN
6. 다음 테스트로 반복
```

---

### Module 1: Calculator — TDD 3법칙 기초

```bash
cd tdd_study/01_calculator
make test
```

**배우는 것:** Red-Green-Refactor 사이클, 에러 코드 패턴

**직접 해보기:**

```c
/* Step 1: calculator.c를 열고 add()를 스텁으로 바꾸기 */
int add(int a, int b) { return 0; }   /* ← 이렇게 변경 */

/* Step 2: make test → RED 확인 */
/* [RUN ] test_add_two_positives_returns_sum ... FAIL
/*     FAIL: expected 3, got 0 */

/* Step 3: 올바른 구현으로 변경 */
int add(int a, int b) { return a + b; }

/* Step 4: make test → GREEN 확인 */
```

**핵심 질문:** `divide()` 함수의 에러 처리를 테스트하려면 왜 `out-parameter(*result)` 패턴을 써야 할까요?
(반환값으로 에러 코드를 쓰면서 동시에 결과값도 반환해야 하므로)

---

### Module 2: Ring Buffer — 경계조건 TDD

```bash
cd tdd_study/02_ring_buffer
make test
```

**배우는 것:** 경계조건 테스트, 상태 기반 테스트, wrap-around 검증

**직접 해보기:**

```c
/* Step 1: ring_buffer.c를 열고 rb_push()를 스텁으로 */
int rb_push(ring_buffer_t *rb, uint8_t data)
{
    (void)rb; (void)data;
    return 0;   /* head 증가 없음, count 증가 없음 */
}

/* Step 2: make test
   test_rb_push_one_item_count_becomes_one ... FAIL
   FAIL: expected 1, got 0   ← count가 증가하지 않음 */

/* Step 3: 구현 추가 — 테스트가 요구하는 것만 */
```

**핵심 질문:** `head == tail`일 때 empty와 full을 어떻게 구분할까요?
(`count` 필드로 구분. head/tail만 쓰면 모호함 발생)

**Wrap-around 테스트 직접 추적:**
```
capacity=3 버퍼:
push A → buf=[A,_,_], head=1, tail=0, count=1
push B → buf=[A,B,_], head=2, tail=0, count=2
push C → buf=[A,B,C], head=0, tail=0, count=3  ← head가 0으로 wrap!
pop   → returns A,     head=0, tail=1, count=2
pop   → returns B,     head=0, tail=2, count=1
push D → buf=[D,B,C], head=1, tail=2, count=2
pop   → returns C,     head=1, tail=0, count=1  ← tail도 wrap!
pop   → returns D,     head=1, tail=1, count=0
```

---

### Module 3: Memory Pool — Refactoring 실습

```bash
cd tdd_study/03_mem_pool
make test
```

**배우는 것:** 복잡한 상태 관리 TDD, Refactoring 안전성

**Refactoring 실습 (이 모듈의 핵심):**

현재 `mem_pool.c`는 **비트맵 기반** free list를 사용합니다.
이것을 **배열 기반 스택** 방식으로 완전히 교체해보세요:

```c
/* 배열 기반 free list 구조체로 교체 */
typedef struct {
    void  *memory;
    size_t block_size;
    size_t num_blocks;
    int    available;
    int    free_stack[MEM_POOL_MAX_BLOCKS];   /* 빈 블록 인덱스 스택 */
    int    stack_top;                          /* 스택 포인터 */
} mem_pool_t;

/* pool_init: 스택을 0,1,2,...,n-1로 초기화 */
/* pool_alloc: free_stack[stack_top--] 반환 */
/* pool_free: free_stack[++stack_top] = index */
```

교체 후 `make test`를 실행하면 **여전히 11 tests GREEN** 이어야 합니다.
이것이 Refactoring의 본질: **외부 동작은 동일, 내부 구현만 변경**.

**핵심 질문:** double-free를 방어하려면 배열 기반에서 어떻게 구현할까요?
(이미 free 상태인 인덱스가 스택에 중복으로 들어가지 않도록 체크)

---

### Module 4: LED State Machine — 의존성 주입

```bash
cd tdd_study/04_state_machine
make test
```

**배우는 것:** 테스트 대역(Test Double), 의존성 주입, 하드웨어 없이 드라이버 테스트

**Fake GPIO 패턴 이해:**

```c
/* 실제 GPIO (하드웨어 직접 접근) */
void real_gpio_write(int pin, int value) {
    volatile uint32_t *reg = gpio_base + pin * 4;
    *reg = value;   ← 실제 하드웨어, 테스트 불가
}

/* Fake GPIO (테스트 대역) */
static int fake_pin_state   = 0;
static int fake_write_count = 0;

void fake_gpio_write(int pin, int value) {
    (void)pin;
    fake_pin_state = value;     ← 메모리에 저장
    fake_write_count++;          ← 호출 횟수 추적
}

/* 테스트에서 검증 */
void test_fsm_set_on_drives_gpio_high(void) {
    led_fsm_set(LED_ON);
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);   ← 핀 값 검증
    TEST_ASSERT_EQUAL_INT(1, fake_write_count);          ← 호출 횟수 검증
}
```

**직접 해보기:** `led_fsm.c`의 모든 함수를 빈 스텁으로 바꾸고 TDD 사이클을 직접 따라가세요.

**핵심 질문:** `led_fsm_reset()`이 없으면 무슨 문제가 생길까요?
(전역 상태가 테스트 간에 누적 → FIRST의 I 위반 → 테스트 순서에 따라 결과 변동)

---

## 8. TDD 핵심 스킬 정리

### 스킬 1: 경계조건을 항상 테스트하라

버그의 80%는 경계에서 발생합니다:

| 상황 | 테스트해야 할 경계 |
|------|-------------------|
| 컬렉션 | 비어있을 때, 하나일 때, 가득 찼을 때 |
| 숫자 | 0, 음수, 최대값, 최소값 |
| 포인터 | NULL, 범위 밖, 이중 해제 |
| 상태 머신 | 각 상태에서 모든 이벤트 |

### 스킬 2: 테스트 하나에 Act 하나

테스트는 **한 가지 동작**만 검증해야 합니다.
여러 동작을 한 테스트에 넣으면 실패 원인을 찾기 어려워집니다.

```c
/* 나쁜 예: 너무 많은 동작 */
void test_ring_buffer(void) {
    ring_buffer_t rb;
    rb_init(&rb, 4);
    rb_push(&rb, 1);
    rb_push(&rb, 2);
    rb_pop(&rb, &val);
    rb_push(&rb, 3);
    /* ... 30줄 더 ... */
    TEST_ASSERT(...);
}

/* 좋은 예: 하나의 동작 집중 */
void test_rb_push_one_item_count_becomes_one(void) { ... }
void test_rb_pop_returns_pushed_value(void) { ... }
void test_rb_push_when_full_returns_error(void) { ... }
```

### 스킬 3: 테스트가 API를 설계한다

테스트를 먼저 쓰면 "사용자 관점"에서 API를 설계하게 됩니다.

```c
/* 테스트를 먼저 쓰면서 자연스럽게 나온 API */
void test_divide_by_zero_returns_error(void) {
    int result = 0;
    calc_err_t err = divide(10, 0, &result);
    TEST_ASSERT_EQUAL_INT(CALC_DIV_ZERO, err);
}
/* → 반환값을 에러코드로, 결과는 out-parameter로 → 사용하기 편한 API */
```

### 스킬 4: 의존성을 주입하라 (Dependency Injection)

테스트하기 어려운 코드는 대부분 의존성이 하드코딩되어 있습니다.

```c
/* 테스트 불가 */
void led_blink(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);   /* 직접 호출 */
}

/* 테스트 가능 — 의존성을 파라미터로 받음 */
void led_blink(gpio_ops_t *ops, int pin) {
    ops->gpio_write(pin, 1);   /* 테스트에서는 fake ops 전달 */
}
```

### 스킬 5: Refactoring은 GREEN 상태에서만

Refactoring은 반드시 모든 테스트가 GREEN인 상태에서 시작하세요.

```
RED 상태에서 Refactoring × → 무엇이 문제인지 알 수 없음
GREEN 상태에서 Refactoring ○ → 변경 후에도 GREEN이면 안전
```

---

## 9. 자주 하는 실수

### 실수 1: 테스트를 나중에 작성

```
코드 작성 완료 후 테스트 작성 → TDD가 아님
이미 완성된 코드의 테스트는 설계를 개선하지 못함
```

### 실수 2: 한 번에 너무 많은 테스트

```
10개 테스트 작성 → 구현 → 7개 FAIL
어디서부터 시작해야 할지 모름

올바른 방법: 테스트 1개 → RED → GREEN → 다음 테스트 1개
```

### 실수 3: GREEN을 건너뛰고 Refactoring

```
RED → (GREEN 없이) REFACTOR → 무엇이 문제인지 파악 불가
반드시 RED → GREEN → REFACTOR 순서 지키기
```

### 실수 4: 테스트에서 구현 세부사항 검증

```c
/* 나쁜 예: 비트맵의 내부 값을 직접 검증 */
void test_pool_alloc(void) {
    pool_alloc(&pool);
    TEST_ASSERT_EQUAL_INT(0, pool.bitmap[0] & 0x01);   /* 내부 구현 노출! */
}

/* 좋은 예: 공개 API의 동작만 검증 */
void test_pool_alloc_decreases_available(void) {
    pool_alloc(&pool);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS - 1, pool_available(&pool));
}
```

내부 구현을 검증하면 Refactoring할 때 테스트도 바꿔야 합니다.
이러면 테스트가 안전망이 아니라 짐이 됩니다.

### 실수 5: 테스트에서 공유 전역 상태 사용

```c
/* 나쁜 예 */
static ring_buffer_t g_rb;   /* 전역 공유 */
void test_a(void) { rb_push(&g_rb, 1); }   /* 상태 남김 */
void test_b(void) { /* g_rb에 1이 이미 있음 → 테스트 순서 의존 */ }

/* 좋은 예: 각 테스트가 자체 상태 소유 */
void test_a(void) { ring_buffer_t rb; rb_init(&rb, 4); ... rb_destroy(&rb); }
void test_b(void) { ring_buffer_t rb; rb_init(&rb, 4); ... rb_destroy(&rb); }
```

---

## 빠른 참조

```bash
# 각 모듈 테스트 실행
cd tdd_study/01_calculator  && make test
cd tdd_study/02_ring_buffer && make test
cd tdd_study/03_mem_pool    && make test
cd tdd_study/04_state_machine && make test

# 빌드 결과물 정리
make clean
```

```c
/* TDD 체크리스트 */
// [ ] 테스트를 먼저 작성했는가? (Law 1)
// [ ] 테스트가 RED인 것을 확인했는가? (Law 2)
// [ ] 최소한의 코드로 GREEN을 만들었는가? (Law 3)
// [ ] REFACTOR 후에도 GREEN인가?
// [ ] 테스트 이름이 명세를 설명하는가?
// [ ] 경계조건을 테스트했는가?
// [ ] 테스트가 서로 독립적인가? (FIRST-I)
```
