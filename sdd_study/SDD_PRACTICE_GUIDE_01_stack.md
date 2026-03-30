# SDD 실습 가이드 — Stack (프롬프트 기반)

> AI에게 프롬프트를 입력하면서 SDD 사이클을 직접 체험하는 실습입니다.
> 각 단계의 프롬프트를 그대로 복사해서 Claude에게 입력하세요.

---

## 실습 구조

```
STEP 1  인터페이스 설계 — API를 명세하기 전에 "무엇을 만드는가" 정의
   ↓
STEP 2  형식적 명세 — @pre / @post / @invariant 계약 주석 작성
   ↓
STEP 3  계약 코드 — contract.h + stack.h (구현 없음)
            3-1. contract.h (REQUIRE/ENSURE/INVARIANT 매크로)
            3-2. stack.h (계약 주석 + 함수 선언)
            3-3. Makefile
   ↓
STEP 4  명세 기반 테스트 — 명세의 속성을 테스트로 변환
            4-1. 속성 목록 도출
            4-2. test_stack.c 작성 (RED)
   ↓
STEP 5  구현 — REQUIRE/ENSURE 포함한 stack.c 작성 (GREEN)
   ↓
STEP 6  계약 위반 실험 — 사전조건을 의도적으로 위반해서 abort 확인
```

---

## TDD와 SDD의 흐름 비교

```
TDD:  [실패 테스트] → [최소 구현] → [리팩터링]
SDD:  [인터페이스] → [형식적 명세] → [명세 기반 테스트] → [계약 포함 구현]
```

> SDD는 테스트보다 **명세**가 먼저다.
> 명세가 완성되면 테스트는 명세를 "코드로 번역"한 것이 된다.

---

## STEP 1. 인터페이스 설계

### 목표
구현도, 테스트도, 명세도 아직 없습니다.
"이 모듈을 사용하는 사람에게 어떤 함수를 줄 것인가"만 결정합니다.

### 프롬프트

```
정수를 저장하는 고정 크기 스택(Stack) C 라이브러리의 인터페이스를 설계해줘.

조건:
- 기능: push, pop, peek, is_empty, is_full, size
- 고정 크기 배열 기반 (동적 할당 없음)
- 에러는 반환값으로 표현 (Linux 커널 스타일: 0=성공, 음수=에러)
- 구현 코드는 작성하지 말 것
- 아래 형식으로 각 함수의 이름, 파라미터, 반환값, 한 줄 설명만 작성

| 함수 | 파라미터 | 반환값 | 설명 |
```

### 확인 포인트

- [ ] push, pop, peek, is_empty, is_full, size 6개 함수가 있다
- [ ] 에러 반환값이 음수(int)로 정의되어 있다
- [ ] 구현 코드가 없다 — 인터페이스만 있다

### 왜 이 단계가 중요한가

> 명세(Specification)는 인터페이스에 붙인다.
> 인터페이스가 불분명하면 무엇을 명세해야 할지 모른다.
> 인터페이스 설계가 SDD의 출발점이다.

---

## STEP 2. 형식적 명세 작성

### 목표
각 함수의 **계약(Contract)**을 @pre / @post / @invariant / @modifies로 명시합니다.
이 단계가 SDD의 핵심입니다. 코드는 아직 없습니다.

### 왜 3가지 계약이 필요한가

```
@pre  (사전조건) — 호출자가 지켜야 할 조건. 위반 시 → 호출자 버그
@post (사후조건) — 함수가 보장하는 결과. 위반 시 → 구현자 버그
@invariant       — 모듈이 항상 유지해야 하는 상태

누가 버그인지가 명확해진다 → 디버깅 비용 절감
```

### 프롬프트 2-1: push 명세

```
stack_push() 함수의 형식적 명세를 작성해줘.

함수 시그니처:
  int stack_push(stack_t *s, int data);

아래 형식을 사용:
  @pre  (사전조건 — 호출자가 보장해야 하는 것)
  @post (사후조건 — 함수가 보장하는 것)
  @modifies (변경되는 필드)
  @return (반환값 의미)

추가 조건:
- OLD_SIZE 표기로 호출 전 크기를 참조할 것
- 스택이 가득 찼을 때의 계약도 포함
- 코드는 작성하지 말 것
```

### 확인 포인트 (push)

- [ ] `@pre s != NULL` 이 있다
- [ ] `@pre !stack_is_full(s)` 가 있다
- [ ] `@post stack_size(s) == OLD_SIZE + 1` 이 있다
- [ ] `@post stack_peek(s) == data` 가 있다
- [ ] `@modifies` 에 변경 필드가 명시되어 있다

---

### 프롬프트 2-2: pop 명세

```
stack_pop() 함수의 형식적 명세를 작성해줘.

함수 시그니처:
  int stack_pop(stack_t *s, int *out);

아래 형식을 사용:
  @pre / @post / @modifies / @return

추가 조건:
- 스택이 비어있을 때와 아닐 때를 구분해서 명세
- OLD_TOP 표기로 호출 전 top 값을 참조할 것
- out 파라미터의 상태 변화도 명세에 포함
```

### 확인 포인트 (pop)

- [ ] `@pre !stack_is_empty(s)` 가 있다
- [ ] `@post *out == OLD_TOP` 이 있다
- [ ] `@post stack_size(s) == OLD_SIZE - 1` 이 있다

---

### 프롬프트 2-3: 스택 불변식 정의

```
stack_t 구조체의 불변식(Invariant)을 정의해줘.

stack_t 구조체는 아래 필드를 가진다:
  int   data[STACK_MAX_SIZE];  // 데이터 배열
  int   top;                   // 다음 push 위치 인덱스 (0-based)
  int   capacity;              // 최대 크기

아래 형식으로 수학적 속성을 3개 이상 작성:
  @invariant [조건] — [이유]

코드 없이 조건식만 작성.
```

### 확인 포인트 (불변식)

- [ ] `0 <= s->top <= s->capacity` 형태의 범위 불변식이 있다
- [ ] `s->capacity == STACK_MAX_SIZE` 가 있다
- [ ] push/pop 후에도 이 불변식들이 유지되어야 한다고 설명하고 있다

---

## STEP 3. 계약 코드 작성

### 목표
명세를 코드로 옮깁니다. 구현(`stack.c`)은 아직 없습니다.

### 프롬프트 3-1: contract.h 작성

```
C 언어에서 Design by Contract를 구현하는 contract.h 매크로 파일을 작성해줘.

조건:
- 매크로: REQUIRE(cond), ENSURE(cond), INVARIANT(cond)
- 조건 위반 시: stderr에 메시지 출력 후 abort()
- 출력 형식: "Precondition failed: [조건식]\n  at [파일]:[라인]"
- NDEBUG가 정의되면 세 매크로 모두 ((void)0) 으로 치환
- include guard 포함
```

### 확인 포인트

- [ ] `REQUIRE`, `ENSURE`, `INVARIANT` 세 매크로가 있다
- [ ] 조건 위반 시 `abort()`가 호출된다
- [ ] 위반한 조건식 문자열이 출력에 포함된다 (`#cond`)
- [ ] `__FILE__`, `__LINE__` 으로 위치가 출력된다
- [ ] `#ifdef NDEBUG` 분기가 있다

---

### 프롬프트 3-2: stack.h 작성 (계약 주석 포함)

```
stack.h 헤더 파일을 작성해줘.

조건:
- STEP 2에서 정의한 @pre / @post / @invariant 주석을 모든 함수에 포함
- 함수 선언만 있고 구현은 없음
- stack_t 구조체 정의 포함 (data 배열, top, capacity 필드)
- STACK_MAX_SIZE는 8로 정의
- 에러 코드: STACK_OK=0, STACK_EMPTY=-1, STACK_FULL=-2, STACK_INVAL=-3
- #include "contract.h" 포함
- include guard 포함
```

### 확인 포인트

- [ ] 모든 함수에 `@pre`, `@post` 주석이 있다
- [ ] `stack_t` 구조체가 정의되어 있다
- [ ] 에러 코드 4개가 enum 또는 #define으로 있다
- [ ] `contract.h`를 include하고 있다

---

### 프롬프트 3-3: Makefile 작성

```
sdd_study/01_stack/ 프로젝트를 빌드하는 Makefile을 작성해줘.

조건:
- 컴파일러: gcc
- 컴파일 옵션: -Wall -Wextra -std=c11 -g
- 빌드 대상: test_runner
- 소스: test_stack.c stack.c
- 헤더 경로: . 포함 (-I.)
- make test: 빌드 후 ./test_runner 자동 실행
- make clean: *.o와 test_runner 삭제
- .PHONY 선언 포함
```

### 이 시점의 파일 구조

```
sdd_study/01_stack/
├── contract.h     ← REQUIRE/ENSURE/INVARIANT 매크로
├── stack.h        ← 계약 주석 + 함수 선언 (구현 없음)
└── Makefile
```

> 아직 `stack.c`도 `test_stack.c`도 없다.
> 명세와 인터페이스만 완성된 상태 — 이것이 SDD의 특징이다.

---

## STEP 4. 명세 기반 테스트 작성

### 목표
STEP 2에서 정의한 명세의 **속성**을 테스트로 변환합니다.
"어떤 입력을 줬을 때 어떤 값이 나온다" 가 아니라
"어떤 **속성**이 항상 성립하는가"를 테스트합니다.

### 프롬프트 4-1: 속성 목록 도출

```
STEP 2에서 정의한 stack 명세(@pre, @post, @invariant)를 보고
테스트할 수 있는 속성(Property) 목록을 뽑아줘.

속성 예시 형식:
  속성 이름: push_increases_size
  검증 방법: push 후 size가 1 증가했는지 확인
  관련 명세: @post stack_size(s) == OLD_SIZE + 1

아래 분류로 나눠줘:
  1. push/pop 대칭 속성
  2. size 속성
  3. LIFO(후입선출) 속성
  4. 경계값 속성 (empty/full)
  5. 불변식 속성
```

### 확인 포인트

- [ ] 5개 이상의 속성이 도출되었다
- [ ] 각 속성이 어떤 명세(@pre/@post/@invariant)에서 왔는지 연결되어 있다
- [ ] "push 후 pop하면 같은 값" 같은 LIFO 속성이 포함되어 있다

---

### 프롬프트 4-2: test_stack.c 작성 (RED)

```
위 속성 목록을 C 테스트 코드로 작성해줘.

조건:
- 파일명: test_stack.c
- 테스트 프레임워크: 아래 매크로 사용 (tdd_study/framework/test_framework.h 사용)
    TEST_ASSERT_EQUAL_INT(expected, actual)
    RUN_TEST(test_func_name)
    TEST_SUMMARY()
    TEST_EXIT_CODE
- 헤더: #include "../../tdd_study/framework/test_framework.h"
         #include "stack.h"
- stack.c는 아직 없음 (RED 상태)
- 각 속성을 하나의 테스트 함수로 작성
- 테스트 이름: test_[속성이름] 형식
- main()에서 RUN_TEST로 모든 테스트 실행
```

### RED 확인

```bash
cd sdd_study/01_stack
make test
```

### 기대 결과 (RED)

```
gcc ... -c test_stack.c ...
test_stack.c:2:10: fatal error: stack.h: No such file or directory
```

또는 stack.h가 있다면:

```
undefined reference to 'stack_push'
undefined reference to 'stack_pop'
...
```

> **이것이 RED입니다.**
> TDD의 RED와 같지만 이유가 다릅니다.
> TDD는 "실패 테스트를 먼저 쓰기 위해" RED를 만들고,
> SDD는 "명세가 완성된 후 구현이 없어서" 자연스럽게 RED입니다.

---

## STEP 5. 구현 (GREEN)

### 목표
명세에서 정의한 계약(`REQUIRE`, `ENSURE`)을 포함해서 구현합니다.
계약이 코드 안에 살아있어야 합니다.

### 프롬프트 5-1: stack.c 작성

```
stack.c를 작성해줘.

조건:
- stack.h의 @pre를 모두 REQUIRE()로 구현
- stack.h의 @post를 모두 ENSURE()로 구현
- ENSURE 검증을 위해 함수 시작 시 OLD_SIZE 등 이전 상태를 저장
- 불변식은 각 함수 끝에서 INVARIANT()로 검사
- TDD Law 3 적용: 테스트를 통과할 최소 구현 (불필요한 기능 추가 금지)
- 함수마다 어느 @pre/@post가 어느 REQUIRE/ENSURE에 대응하는지 주석으로 표시
```

### GREEN 확인

```bash
make test
```

### 기대 결과 (GREEN)

```
[RUN ] test_push_increases_size              PASS
[RUN ] test_pop_decreases_size               PASS
[RUN ] test_push_pop_returns_same_value      PASS
[RUN ] test_lifo_order_preserved             PASS
[RUN ] test_size_zero_on_empty_stack         PASS
[RUN ] test_invariant_after_push_pop         PASS
...
======================================================
  ALL PASSED  |  Total: N  |  Passed: N  |  Failed:  0
======================================================
```

---

## STEP 6. 계약 위반 실험

### 목표
SDD에서 가장 강력한 부분을 직접 체험합니다.
사전조건을 **의도적으로 위반**해서 `REQUIRE`가 `abort()`를 호출하는 것을 확인합니다.

### 프롬프트 6-1: 사전조건 위반 테스트 코드

```
아래 두 가지 계약 위반 상황을 실험하는 C 코드를 작성해줘.
파일명: test_contract_violation.c

실험 1: 가득 찬 스택에 push
  - STACK_MAX_SIZE만큼 push한 뒤 한 번 더 push
  - 기대: REQUIRE 실패 메시지 출력 후 프로세스 abort

실험 2: 빈 스택에서 pop
  - 비어있는 스택에서 pop 호출
  - 기대: REQUIRE 실패 메시지 출력 후 프로세스 abort

조건:
- main()에서 실험 1과 2를 각각 실행
- 각 실험 전에 어떤 계약 위반인지 printf로 출력
- Makefile에 별도 타겟 추가: make contract_test
```

### 실험 실행

```bash
make contract_test
```

### 기대 결과

```
=== 실험 1: 가득 찬 스택에 push ===
Precondition failed: !stack_is_full(s)
  at stack.c:34
Aborted (core dumped)
```

### 확인 포인트

- [ ] `REQUIRE` 위반 메시지에 **조건식 문자열**이 포함되어 있다
- [ ] **파일명과 라인 번호**가 출력된다
- [ ] 프로세스가 `abort()`로 종료된다 (exit code 134 또는 SIGABRT)
- [ ] 일반 `make test`의 테스트는 여전히 전부 PASS다

### abort vs 에러 코드 반환 — 언제 어떻게?

```
REQUIRE (사전조건 위반) → abort()
  이유: 호출자 버그. 계속 실행하면 더 위험한 상태로 진행됨.
  디버그 빌드에서만 동작. 릴리즈(NDEBUG)에서는 제거.

에러 코드 반환 (-EINVAL 등) → 정상적인 에러 처리
  이유: 외부 입력, 런타임 조건 — 호출자가 알 수 없었던 상황.
  예: 파일이 없음, 네트워크 오류, 리소스 부족.

커널 스타일 판단 기준:
  "호출자가 사전에 확인할 수 있었는가?"
    YES → REQUIRE (사전조건)
    NO  → 에러 코드 반환
```

---

## 최종 파일 구조

```
sdd_study/01_stack/
├── contract.h                  ← REQUIRE/ENSURE/INVARIANT 매크로
├── stack.h                     ← 계약 주석(@pre/@post/@invariant) + 선언
├── stack.c                     ← REQUIRE/ENSURE/INVARIANT 포함한 구현
├── test_stack.c                ← 명세 기반 속성 테스트
├── test_contract_violation.c   ← 계약 위반 실험
├── Makefile
├── stack.o
├── test_stack.o
└── test_runner
```

---

## 실습 체크리스트

### STEP 1: 인터페이스 설계
- [ ] 6개 함수(push, pop, peek, is_empty, is_full, size)의 시그니처가 결정되었다
- [ ] 에러 반환 규칙이 정해졌다

### STEP 2: 형식적 명세
- [ ] push의 @pre, @post를 작성했다
- [ ] pop의 @pre, @post를 작성했다
- [ ] stack_t의 @invariant를 3개 이상 정의했다
- [ ] OLD_SIZE/OLD_TOP 표기로 상태 변화를 표현했다

### STEP 3: 계약 코드
- [ ] contract.h의 세 매크로가 동작한다
- [ ] stack.h의 모든 함수에 계약 주석이 있다
- [ ] Makefile로 `make test`가 실행된다

### STEP 4: 명세 기반 테스트
- [ ] 속성 5개 이상을 도출했다
- [ ] 각 속성이 테스트 함수로 변환되었다
- [ ] `make test`가 RED (링크 에러 또는 컴파일 에러)다

### STEP 5: 구현
- [ ] `REQUIRE`가 @pre를 구현한다
- [ ] `ENSURE`가 @post를 구현한다
- [ ] 각 함수 끝에 `INVARIANT` 검사가 있다
- [ ] `make test`가 ALL PASSED다

### STEP 6: 계약 위반 실험
- [ ] 가득 찬 스택에 push 시 REQUIRE 메시지와 abort를 확인했다
- [ ] 빈 스택에서 pop 시 REQUIRE 메시지와 abort를 확인했다
- [ ] 위반 메시지에 조건식, 파일명, 라인 번호가 포함되어 있다

---

## 핵심 규칙 요약

| 단계 | 하는 일 | 하면 안 되는 일 |
|------|---------|----------------|
| **인터페이스** | API 목록 결정 | 구현, 명세 작성 |
| **명세** | @pre/@post/@invariant 정의 | 코드 작성 |
| **계약 코드** | contract.h + stack.h | stack.c 작성 |
| **테스트** | 속성을 테스트 함수로 변환 | 구현 코드 작성 |
| **구현** | REQUIRE/ENSURE 포함 구현 | 명세 없는 기능 추가 |

> **SDD의 핵심:**
> 명세가 없는 코드는 작성하지 않는다.
> 호출자 버그(REQUIRE)와 구현자 버그(ENSURE)를 항상 구분한다.
> 계약은 디버그 빌드에서 런타임으로 살아있어야 한다.
