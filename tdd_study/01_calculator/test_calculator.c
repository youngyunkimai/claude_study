/*
 * test_calculator.c — Module 1: TDD 3법칙 기초
 *
 * ============================================================
 * 이 파일을 먼저 읽으세요 — 테스트가 명세입니다
 * ============================================================
 *
 * TDD에서 테스트는 단순히 코드를 검증하는 것이 아닙니다.
 * 테스트는 "구현이 무엇을 해야 하는지"를 정의하는 명세입니다.
 *
 * 실습 방법:
 *   1. calculator.c에서 모든 함수를 스텁(return 0)으로 바꾸세요.
 *   2. $ make test  → 테스트가 실패하는 것 확인 (RED)
 *   3. 테스트를 하나씩 보면서, 통과할 최소 코드를 작성하세요.
 *   4. $ make test  → 테스트가 통과하는 것 확인 (GREEN)
 *   5. 코드를 정리하고 다시 $ make test (REFACTOR → still GREEN)
 *
 * ============================================================
 * 테스트 이름 규칙: test_[단위]_[시나리오]_[기대결과]
 * ============================================================
 */

#include "../framework/test_framework.h"
#include "calculator.h"

/* ----------------------------------------------------------
 * ADD 테스트
 *
 * TDD 스킬: 양수, 음수, 0, 경계값을 각각 다른 테스트로 분리.
 * 하나의 테스트에 모든 케이스를 넣지 마세요.
 * 실패했을 때 "어느 케이스가 왜 실패했는지" 바로 알 수 있어야 합니다.
 * ---------------------------------------------------------- */

/* 가장 단순한 케이스부터 시작 — TDD Law 2: 실패하기에 충분한 만큼만 */
void test_add_two_positives_returns_sum(void)
{
    TEST_ASSERT_EQUAL_INT(3, add(1, 2));
}

void test_add_negative_numbers_returns_sum(void)
{
    TEST_ASSERT_EQUAL_INT(-5, add(-2, -3));
}

void test_add_zero_returns_same_number(void)
{
    TEST_ASSERT_EQUAL_INT(7, add(7, 0));
    TEST_ASSERT_EQUAL_INT(7, add(0, 7));
}

void test_add_positive_and_negative_returns_difference(void)
{
    TEST_ASSERT_EQUAL_INT(1, add(3, -2));
}

/* ----------------------------------------------------------
 * SUBTRACT 테스트
 * ---------------------------------------------------------- */

void test_subtract_smaller_from_larger_returns_positive(void)
{
    TEST_ASSERT_EQUAL_INT(3, subtract(5, 2));
}

void test_subtract_larger_from_smaller_returns_negative(void)
{
    TEST_ASSERT_EQUAL_INT(-3, subtract(2, 5));
}

/* ----------------------------------------------------------
 * MULTIPLY 테스트
 * ---------------------------------------------------------- */

void test_multiply_two_positives_returns_product(void)
{
    TEST_ASSERT_EQUAL_INT(6, multiply(2, 3));
}

/* 0 곱하기: 특별한 수학적 성질 — 별도 테스트로 명시 */
void test_multiply_by_zero_returns_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, multiply(5, 0));
    TEST_ASSERT_EQUAL_INT(0, multiply(0, 5));
}

void test_multiply_negative_numbers_returns_positive(void)
{
    TEST_ASSERT_EQUAL_INT(6, multiply(-2, -3));
}

/* ----------------------------------------------------------
 * DIVIDE 테스트
 *
 * TDD 스킬: 에러 경로(0으로 나누기)를 정상 경로와 분리해서 테스트.
 * 에러 경로가 정상 동작을 방해하면 안 됩니다.
 * ---------------------------------------------------------- */

void test_divide_two_positives_returns_quotient(void)
{
    int result = 0;
    calc_err_t err = divide(10, 2, &result);

    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void test_divide_by_zero_returns_error_code(void)
{
    int result = 0;
    calc_err_t err = divide(10, 0, &result);

    /* 에러 코드를 반환해야 함 */
    TEST_ASSERT_EQUAL_INT(CALC_DIV_ZERO, err);

    /* TDD 스킬: 에러 시 result 값에 의존하지 않는다.
     * result가 쓰레기 값이어도 에러 코드로 판단해야 함. */
}

void test_divide_negative_by_positive_returns_negative_quotient(void)
{
    int result = 0;
    calc_err_t err = divide(-10, 2, &result);

    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-5, result);
}

/* ----------------------------------------------------------
 * main — 테스트 러너
 *
 * RUN_TEST를 한 줄씩 추가하면서 TDD 사이클을 돌리세요.
 * 한 번에 모든 테스트를 추가하지 말고,
 * RED → GREEN → REFACTOR → 다음 테스트 순서로 진행하세요.
 * ---------------------------------------------------------- */
int main(void)
{
    printf("Module 1: Calculator — TDD 3법칙 기초\n");
    printf("------------------------------------------------------\n");

    /* ADD */
    RUN_TEST(test_add_two_positives_returns_sum);
    RUN_TEST(test_add_negative_numbers_returns_sum);
    RUN_TEST(test_add_zero_returns_same_number);
    RUN_TEST(test_add_positive_and_negative_returns_difference);

    /* SUBTRACT */
    RUN_TEST(test_subtract_smaller_from_larger_returns_positive);
    RUN_TEST(test_subtract_larger_from_smaller_returns_negative);

    /* MULTIPLY */
    RUN_TEST(test_multiply_two_positives_returns_product);
    RUN_TEST(test_multiply_by_zero_returns_zero);
    RUN_TEST(test_multiply_negative_numbers_returns_positive);

    /* DIVIDE */
    RUN_TEST(test_divide_two_positives_returns_quotient);
    RUN_TEST(test_divide_by_zero_returns_error_code);
    RUN_TEST(test_divide_negative_by_positive_returns_negative_quotient);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
