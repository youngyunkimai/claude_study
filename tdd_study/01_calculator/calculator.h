/*
 * calculator.h — Module 1: TDD 3법칙 기초
 *
 * ============================================================
 * 이 모듈에서 배우는 것
 * ============================================================
 *   - TDD 3법칙 (Red-Green-Refactor 사이클)
 *   - 테스트가 API 설계를 이끄는 방식
 *   - 에러 코드 패턴 (커널 스타일: 음수 = 에러)
 *
 * ============================================================
 * 실습 순서 (직접 따라해보세요)
 * ============================================================
 *
 * STEP 1 [RED]: 먼저 test_calculator.c를 열어서 첫 번째 테스트를 보세요.
 *   $ make test  →  컴파일 에러 발생 (이것이 RED 상태)
 *   "calculator.h: No such file or directory"
 *
 * STEP 2 [RED]: 이 헤더를 만들었습니다. 다시 시도:
 *   $ make test  →  링크 에러 (구현이 없으므로 여전히 RED)
 *
 * STEP 3 [RED → GREEN]: calculator.c에 최소 구현을 추가하세요.
 *   처음엔 add()가 "return 0;"을 반환하게 해보세요.
 *   $ make test  →  test_add_two_positives_returns_sum FAIL
 *   이제 "return a + b;"로 바꾸면 → PASS (GREEN!)
 *
 * STEP 4 [REFACTOR]: 모든 테스트가 GREEN이면 코드를 다듬으세요.
 *   구조는 바꾸되 테스트는 여전히 GREEN이어야 합니다.
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

/*
 * 에러 코드 — Linux 커널 스타일
 * 성공 = 0, 에러 = 음수값
 * divide()처럼 결과값과 에러를 동시에 반환해야 할 때는
 * out-parameter (*result) 패턴을 사용합니다.
 */
typedef enum {
    CALC_OK       =  0,
    CALC_DIV_ZERO = -1,
} calc_err_t;

/*
 * 기본 사칙연산
 *
 * TDD 관점: 이 선언들이 있어야 테스트가 컴파일됩니다 (Law 2).
 * calculator.c에서 먼저 스텁(return 0)으로 구현하고,
 * 테스트를 하나씩 통과시키며 완성하세요.
 */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);

/*
 * divide — 나눗셈 (0으로 나누기 에러 처리 포함)
 *
 * @param a      피제수 (dividend)
 * @param b      제수 (divisor)
 * @param result 결과를 저장할 포인터 (b != 0일 때만 유효)
 * @return       CALC_OK 또는 CALC_DIV_ZERO
 *
 * 사용 예:
 *   int result;
 *   if (divide(10, 2, &result) == CALC_OK)
 *       printf("%d\n", result);  // 5
 */
calc_err_t divide(int a, int b, int *result);

#endif /* CALCULATOR_H */
