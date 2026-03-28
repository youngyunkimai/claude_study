# TDD 실습 가이드 — Calculator (프롬프트 기반)

> AI에게 프롬프트를 입력하면서 TDD 사이클을 직접 체험하는 실습입니다.
> 각 단계의 프롬프트를 그대로 복사해서 Claude에게 입력하세요.

---

## 실습 구조

```
STEP 1  요구사항 문서 생성
   ↓
STEP 2  RED — Makefile + 테스트 코드 작성 (컴파일 에러 확인)
            2-1. 테스트 시나리오 목록
            2-2. 테스트 코드 (test_calculator.c)
            2-3. Makefile 생성  ← make test를 쓰려면 필수
            2-4. 헤더 파일만 (calculator.h, 구현 없음)
   ↓
STEP 3  RED → GREEN — 최소 구현으로 테스트 통과
            → test_runner 바이너리 생성 및 직접 실행 확인
   ↓
STEP 4  REFACTOR — 구조 개선 (테스트는 여전히 GREEN)
   ↓
STEP 5  새 요구사항 추가 → 다시 RED부터
```

---

## STEP 1. 요구사항 문서 생성

### 목표
코드를 작성하기 전에 **무엇을 만들어야 하는지** 명확히 합니다.
요구사항이 없으면 테스트를 쓸 수 없고, 테스트가 없으면 TDD를 시작할 수 없습니다.

### 프롬프트

```
계산기 C 라이브러리의 요구사항 문서를 작성해줘.

조건:
- 기능: 정수 덧셈, 뺄셈, 곱셈, 나눗셈
- 0으로 나누기는 에러로 처리
- 에러는 반환값으로 표현 (Linux 커널 스타일: 0=성공, 음수=에러)
- 각 기능별로 정상 동작 / 경계조건 / 에러 케이스를 포함
- 표 형식으로 작성
```

### 확인 포인트
결과물에 아래 항목이 있는지 확인하세요:

- [ ] 각 연산의 입력/출력 명세
- [ ] 에러 코드 정의 (`CALC_OK`, `CALC_DIV_ZERO`)
- [ ] 경계 케이스 (0, 음수, 큰 수)
- [ ] 0으로 나누기 처리 명세

### 왜 이 단계가 중요한가
> TDD Law 1: "실패하는 테스트를 먼저 써라"
> 테스트를 쓰려면 **무엇이 올바른 동작인지** 먼저 알아야 합니다.
> 요구사항 문서가 없으면 테스트가 무엇을 검증해야 하는지 모릅니다.

---

## STEP 2. RED — 테스트 케이스 작성

### 목표
구현 코드 없이 **테스트만** 먼저 작성합니다.
`make test`를 실행하면 컴파일 에러가 나야 합니다. 이것이 **RED** 상태입니다.

### 프롬프트 2-1: 테스트 시나리오 목록 생성

```
위 요구사항을 바탕으로 TDD 테스트 시나리오 목록을 작성해줘.

조건:
- 테스트 이름은 test_[단위]_[시나리오]_[기대결과] 형식
- 정상 동작 / 경계조건 / 에러 케이스로 분류
- 각 시나리오에 입력값과 기대 결과를 명시
- 표 형식으로 작성

예시 형식:
| 테스트 이름 | 입력 | 기대 결과 | 분류 |
```

### 확인 포인트
- [ ] 덧셈 3개 이상 (양수+양수, 음수+양수, +0)
- [ ] 뺄셈 2개 이상
- [ ] 곱셈 3개 이상 (일반, 0 곱하기, 음수×음수)
- [ ] 나눗셈 2개 이상 + 0으로 나누기 1개

---

### 프롬프트 2-2: 테스트 코드 생성

```
위 테스트 시나리오를 C 코드로 작성해줘.

조건:
- 파일명: test_calculator.c
- 테스트 프레임워크: 아래 매크로를 사용

  TEST_ASSERT_EQUAL_INT(expected, actual)
  TEST_ASSERT_EQUAL_INT(CALC_OK, err)
  RUN_TEST(test_func_name)
  TEST_SUMMARY()
  TEST_EXIT_CODE

- 헤더: #include "../framework/test_framework.h"
         #include "calculator.h"
- calculator.h와 calculator.c는 아직 없음 (RED 상태)
- main() 함수에서 RUN_TEST로 모든 테스트 실행
- 각 테스트 함수는 Arrange / Act / Assert 패턴으로 작성
```

### 프롬프트 2-3: Makefile 생성

`make test`를 실행하려면 Makefile이 있어야 합니다.
테스트 코드를 저장하기 **전에** 먼저 Makefile을 준비합니다.

```
이 프로젝트를 빌드하고 테스트하기 위한 Makefile을 작성해줘.

조건:
- 컴파일러: gcc
- 컴파일 옵션: -Wall -Wextra -std=c11 -g
- 빌드 대상(TARGET): test_runner
- 소스 파일: test_calculator.c calculator.c
- 헤더 경로: ../framework 포함 (-I../framework)
- make test: 빌드 후 ./test_runner 자동 실행
- make clean: 오브젝트 파일(*.o)과 test_runner 삭제
- .PHONY 선언 포함
```

생성된 Makefile을 `tdd_study/01_calculator/Makefile`로 저장합니다.

---

### 결과 저장 방법

AI가 생성한 코드를 파일로 저장한 후 테스트를 실행합니다:

```bash
cd tdd_study/01_calculator

# 현재 디렉토리 파일 확인
ls
# Makefile  test_calculator.c  (calculator.h, calculator.c는 아직 없음)

make test
```


### 기대 결과 (RED 확인)

```
gcc -Wall -Wextra -std=c11 -g -I../framework -c -o test_calculator.o test_calculator.c
test_calculator.c:2:10: fatal error: calculator.h: No such file or directory
```

> **이것이 RED입니다.** 컴파일 에러가 나는 것이 정상입니다.
> TDD Law 2: "실패를 증명하기에 충분한 만큼만 테스트를 작성하라"

---

### 프롬프트 2-4: 헤더 파일만 생성 (구현 없음)

```
calculator.h 헤더 파일만 작성해줘.

조건:
- 함수 선언만 포함 (구현 없음)
- add, subtract, multiply, divide 함수
- divide는 결과를 out-parameter(int *result)로 반환
- 에러 코드 enum: CALC_OK=0, CALC_DIV_ZERO=-1
- include guard 포함
```

### calculator.h 저장 후 결과 확인 (여전히 RED)

```bash
make test
```

### 기대 결과 (여전히 RED)

```
undefined reference to 'add'
undefined reference to 'subtract'
...
```

> 헤더가 생겼지만 구현이 없어서 링크 에러가 납니다.
> 여전히 RED — 아직 프로덕션 코드를 작성할 차례가 아닙니다.

---

## STEP 3. GREEN — 최소 구현으로 테스트 통과

### 목표
테스트를 **통과할 만큼만** 구현합니다.
"나중에 필요할 것 같은" 코드는 절대 넣지 않습니다.

### 프롬프트 3-1: 스텁 구현 (의도적으로 틀린 구현)

```
calculator.c를 작성해줘.

조건:
- TDD 연습을 위해 처음엔 의도적으로 틀린 스텁으로 작성
- add(), subtract(), multiply()는 항상 0을 반환
- divide()는 항상 CALC_OK를 반환하고 *result = 0
- 함수 선언은 calculator.h와 일치해야 함
```

### 스텁 저장 후 결과 확인

```bash
make test
```

### 기대 결과 (런타임 RED 확인)

```
[RUN ] test_add_two_positives_returns_sum              FAIL
    FAIL: expected 3, got 0
    at   test_calculator.c:35

[RUN ] test_divide_by_zero_returns_error_code          FAIL
    FAIL: expected -1, got 0
    at   test_calculator.c:87
...
Total: 12 | Passed:  0 | Failed: 12
```

> 이제 **컴파일은 되지만 테스트가 실패**합니다.
> 스텁에서 실제 구현으로 하나씩 채워나갑니다.

---

### 프롬프트 3-2: add() 하나만 먼저 구현

```
calculator.c에서 add() 함수만 올바르게 구현해줘.
나머지 subtract(), multiply(), divide()는 그대로 스텁으로 유지.

TDD Law 3을 적용: 테스트를 통과할 만큼만 작성.
```

### 결과 확인 (부분 GREEN)

```bash
make test
```

```
[RUN ] test_add_two_positives_returns_sum              PASS
[RUN ] test_add_negative_numbers_returns_sum           PASS
[RUN ] test_add_zero_returns_same_number               PASS
[RUN ] test_add_positive_and_negative_returns_difference PASS
[RUN ] test_subtract_smaller_from_larger_returns_positive FAIL
...
Total: 12 | Passed:  4 | Failed: 8
```

> add() 테스트 4개가 PASS로 바뀌었습니다.
> 이 방식으로 **테스트 하나씩** GREEN으로 만들어 나갑니다.

---

### 프롬프트 3-3: 전체 GREEN 구현

```
calculator.c를 완성해줘.

조건:
- TDD Law 3 적용: 테스트를 통과할 최소한의 코드만 작성
- divide()는 b==0일 때 CALC_DIV_ZERO 반환, 결과는 *result에 저장
- overflow 방어, 로깅, 예외처리 등 테스트에 없는 코드는 넣지 말 것
- 현재 test_calculator.c의 12개 테스트를 모두 통과해야 함
```

### 최종 GREEN 확인

```bash
make test
```

### 기대 결과

```
[RUN ] test_add_two_positives_returns_sum              PASS
[RUN ] test_add_negative_numbers_returns_sum           PASS
[RUN ] test_add_zero_returns_same_number               PASS
[RUN ] test_add_positive_and_negative_returns_difference PASS
[RUN ] test_subtract_smaller_from_larger_returns_positive PASS
[RUN ] test_subtract_larger_from_smaller_returns_negative PASS
[RUN ] test_multiply_two_positives_returns_product     PASS
[RUN ] test_multiply_by_zero_returns_zero              PASS
[RUN ] test_multiply_negative_numbers_returns_positive PASS
[RUN ] test_divide_two_positives_returns_quotient      PASS
[RUN ] test_divide_by_zero_returns_error_code          PASS
[RUN ] test_divide_negative_by_positive_returns_negative_quotient PASS

======================================================
  ALL PASSED  |  Total: 12  |  Passed: 12  |  Failed:  0
======================================================
```

> **GREEN 달성!**

---

## test_runner 바이너리 이해

`make test`가 성공하면 다음 파일들이 생성됩니다:

```
tdd_study/01_calculator/
├── Makefile
├── calculator.h
├── calculator.c
├── test_calculator.c
├── calculator.o          ← calculator.c 컴파일 결과 (오브젝트 파일)
├── test_calculator.o     ← test_calculator.c 컴파일 결과 (오브젝트 파일)
└── test_runner           ← 두 .o 파일을 링크한 실행 바이너리
```

### 빌드 흐름

```
calculator.c        →  gcc -c  →  calculator.o      ┐
                                                      ├→ gcc 링크 → test_runner
test_calculator.c   →  gcc -c  →  test_calculator.o  ┘
```

`make test`는 내부적으로 이 과정을 수행합니다:

```bash
# Makefile이 순서대로 실행하는 명령
gcc -Wall -Wextra -std=c11 -g -I../framework -c -o calculator.o calculator.c
gcc -Wall -Wextra -std=c11 -g -I../framework -c -o test_calculator.o test_calculator.c
gcc -Wall -Wextra -std=c11 -g -I../framework -o test_runner calculator.o test_calculator.o
./test_runner
```

### test_runner 직접 실행

`make test` 없이 바이너리를 직접 실행할 수 있습니다:

```bash
# 직접 실행 — make test와 동일한 결과
./test_runner

# 종료 코드 확인 (0=전체 통과, 1=실패 있음)
./test_runner; echo "exit code: $?"

# 특정 테스트 결과만 필터링
./test_runner | grep FAIL
./test_runner | grep PASS
```

직접 실행이 유용한 경우:
- 빌드는 이미 완료된 상태에서 테스트만 재실행할 때
- 출력을 파이프로 연결해서 필터링할 때
- CI/CD 스크립트에서 종료 코드로 성공/실패를 판단할 때

### make clean으로 빌드 결과물 제거

```bash
make clean
# 삭제 대상: *.o 파일, test_runner 바이너리
# 유지 대상: *.c, *.h, Makefile (소스 파일은 건드리지 않음)

ls
# Makefile  calculator.c  calculator.h  test_calculator.c
```

> `make clean` 후 다시 `make test`를 실행하면 처음부터 전체 빌드를 수행합니다.
> 소스 파일을 수정하면 `make`가 변경된 파일만 재컴파일합니다 (증분 빌드).

### 체크리스트 추가 항목
- [ ] `./test_runner`를 직접 실행해서 `make test`와 동일한 결과가 나오는지 확인했다
- [ ] `make clean` 후 `make test`를 다시 실행해서 처음부터 빌드되는지 확인했다

---

## STEP 4. REFACTOR — 구조 개선

### 목표
테스트를 **변경하지 않고** 코드 구조를 개선합니다.
Refactor 후에도 `make test`가 GREEN이어야 합니다.

### 프롬프트 4-1: 리팩토링 대상 파악

```
현재 calculator.c를 분석해서 리팩토링할 부분을 찾아줘.

확인할 항목:
- 중복 코드
- 에러 처리 패턴의 일관성
- 함수 이름과 동작의 일치 여부
- 커널 코딩 스타일 위반 (early return, const 등)

코드를 바꾸지 말고 문제점만 나열해줘.
```

---

### 프롬프트 4-2: Refactoring 실행

```
아래 규칙으로 calculator.c를 리팩토링해줘.

규칙:
- test_calculator.c는 절대 변경하지 않음
- 기능 추가 금지 (테스트에 없는 동작 추가 금지)
- 에러 처리를 early return 패턴으로 통일
- divide()의 NULL 포인터 방어 추가 (테스트에 있다면)
- 주석은 "왜"를 설명, "무엇"은 코드가 말하게

리팩토링 전후 변경 사항을 설명해줘.
```

### Refactor 후 GREEN 재확인

```bash
make test
```

> **반드시 여전히 12 passed, 0 failed** 여야 합니다.
> 테스트가 실패하면 Refactoring이 동작을 바꾼 것입니다 — 즉시 되돌리세요.

---

## STEP 5. 새 요구사항 추가 → 다시 RED부터

### 목표
TDD는 한 번으로 끝나지 않습니다.
새 요구사항이 생기면 **다시 RED부터** 시작합니다.

### 프롬프트 5-1: 새 요구사항 도출

```
계산기에 다음 기능을 추가하려고 해:
- modulo(나머지 연산): a % b, 0으로 나누기 에러 처리 포함
- power(거듭제곱): a^b (b >= 0인 정수)

TDD 방식으로 추가하기 위한 테스트 시나리오를 먼저 작성해줘.
각 함수당 최소 3개의 테스트 케이스 포함.
```

---

### 프롬프트 5-2: 새 테스트 추가 (RED)

```
위 시나리오를 test_calculator.c에 추가할 테스트 함수 코드를 작성해줘.

조건:
- 기존 테스트는 변경하지 않음
- modulo(), power() 함수는 아직 없음 (RED 상태 의도)
- 테스트 함수만 작성, calculator.h/calculator.c 변경 없음
- main()에 추가할 RUN_TEST 줄도 포함
```

```bash
# 테스트 파일에 추가 후
make test
# 기대: 기존 12개 PASS + 새 테스트 컴파일 에러 (RED)
```

---

### 프롬프트 5-3: 새 기능 GREEN 구현

```
modulo()와 power() 함수를 구현해줘.

조건:
- calculator.h에 선언 추가
- calculator.c에 구현 추가
- TDD Law 3: 새 테스트를 통과할 만큼만 작성
- 기존 12개 테스트도 여전히 통과해야 함
```

```bash
make test
# 기대: 전체 테스트 ALL PASSED
```

---

## 실습 체크리스트

각 단계를 완료하면 체크하세요.

### STEP 1: 요구사항
- [ ] 요구사항 문서에 에러 코드 enum이 정의되어 있다
- [ ] 경계 케이스(0, 음수)가 명시되어 있다

### STEP 2: RED
- [ ] Makefile이 생성되어 있다
- [ ] test_calculator.c만 있고 calculator.h, calculator.c는 없는 상태에서 `make test`가 컴파일 에러로 실패한다
- [ ] calculator.h를 추가한 후 링크 에러가 난다 (여전히 RED)
- [ ] 테스트 이름이 `test_[단위]_[시나리오]_[기대결과]` 형식이다

### STEP 3: GREEN
- [ ] 스텁 구현으로 런타임 FAIL을 확인했다
- [ ] 테스트를 하나씩 통과시키며 구현했다 (한 번에 전부 구현 금지)
- [ ] `make test` 결과가 12 passed, 0 failed 이다
- [ ] `./test_runner`를 직접 실행해서 동일한 결과를 확인했다
- [ ] `make clean` 후 `make test`로 전체 재빌드를 확인했다

### STEP 4: REFACTOR
- [ ] Refactoring 전에 GREEN 상태를 확인했다
- [ ] test_calculator.c를 변경하지 않았다
- [ ] Refactoring 후 `make test`가 여전히 12 passed, 0 failed 이다

### STEP 5: 새 요구사항
- [ ] 새 테스트를 먼저 추가하고 RED를 확인했다
- [ ] 기존 테스트를 건드리지 않았다
- [ ] 새 기능 추가 후 전체 테스트가 PASS 이다

---

## 핵심 규칙 요약

| 단계 | 하는 일 | 하면 안 되는 일 |
|------|---------|----------------|
| **RED** | 테스트 작성, 실패 확인 | 구현 코드 작성 |
| **GREEN** | 최소 구현으로 통과 | 기능 추가, 미래 대비 코드 |
| **REFACTOR** | 구조 개선 | 테스트 변경, 기능 추가 |

> **TDD의 핵심:**
> 테스트가 없는 코드는 작성하지 않는다.
> 실패하는 테스트가 없으면 코드를 작성하지 않는다.
> 테스트를 통과하면 즉시 멈추고 다음 테스트로 간다.
