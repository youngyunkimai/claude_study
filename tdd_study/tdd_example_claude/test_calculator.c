#include <limits.h>
#include "test_framework.h"
#include "calculator.h"

/* =========================================================
 * calc_add
 * ========================================================= */

/* --- 정상 동작 --- */

void test_add_positive_numbers_returns_sum(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(1, 2, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void test_add_zero_and_zero_returns_zero(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(0, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_add_negative_and_positive_returns_sum(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(-3, 5, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(2, result);
}

void test_add_two_negatives_returns_sum(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(-3, -4, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-7, result);
}

/* --- 경계 조건 --- */

void test_add_int_max_and_zero_returns_int_max(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(INT_MAX, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(INT_MAX, result);
}

void test_add_int_min_and_zero_returns_int_min(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_add(INT_MIN, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(INT_MIN, result);
}

/* --- 에러 케이스 --- */

void test_add_null_result_returns_einval(void)
{
    /* Arrange */
    int err;

    /* Act */
    err = calc_add(1, 2, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_EINVAL, err);
}

/* =========================================================
 * calc_sub
 * ========================================================= */

/* --- 정상 동작 --- */

void test_sub_larger_minus_smaller_returns_diff(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(5, 3, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(2, result);
}

void test_sub_zero_minus_zero_returns_zero(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(0, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_sub_smaller_minus_larger_returns_negative(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(3, 5, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-2, result);
}

void test_sub_two_negatives_returns_diff(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(-3, -4, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(1, result);
}

/* --- 경계 조건 --- */

void test_sub_int_min_minus_zero_returns_int_min(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(INT_MIN, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(INT_MIN, result);
}

void test_sub_zero_minus_int_min_returns_int_min_plus_one(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_sub(0, INT_MIN + 1, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-(INT_MIN + 1), result);
}

/* --- 에러 케이스 --- */

void test_sub_null_result_returns_einval(void)
{
    /* Arrange */
    int err;

    /* Act */
    err = calc_sub(5, 3, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_EINVAL, err);
}

/* =========================================================
 * calc_mul
 * ========================================================= */

/* --- 정상 동작 --- */

void test_mul_two_positives_returns_product(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(3, 4, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(12, result);
}

void test_mul_by_zero_returns_zero(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(0, 999, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_mul_negative_and_positive_returns_negative(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(-3, 4, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-12, result);
}

void test_mul_two_negatives_returns_positive(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(-3, -4, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(12, result);
}

/* --- 경계 조건 --- */

void test_mul_one_and_int_max_returns_int_max(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(1, INT_MAX, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(INT_MAX, result);
}

void test_mul_zero_and_int_min_returns_zero(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_mul(0, INT_MIN, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(0, result);
}

/* --- 에러 케이스 --- */

void test_mul_null_result_returns_einval(void)
{
    /* Arrange */
    int err;

    /* Act */
    err = calc_mul(3, 4, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_EINVAL, err);
}

/* =========================================================
 * calc_div
 * ========================================================= */

/* --- 정상 동작 --- */

void test_div_even_division_returns_quotient(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(10, 2, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void test_div_truncates_remainder(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(7, 2, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void test_div_zero_dividend_returns_zero(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(0, 5, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_div_negative_dividend_returns_negative(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(-9, 3, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-3, result);
}

/* --- 경계 조건 --- */

void test_div_int_min_by_one_returns_int_min(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(INT_MIN, 1, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(INT_MIN, result);
}

void test_div_same_values_returns_one(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(INT_MAX, INT_MAX, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(1, result);
}

/* --- 에러 케이스 --- */

void test_div_by_zero_returns_edom(void)
{
    /* Arrange */
    int result;
    int err;

    /* Act */
    err = calc_div(10, 0, &result);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_DIV_ZERO, err);
}

void test_div_null_result_returns_einval(void)
{
    /* Arrange */
    int err;

    /* Act */
    err = calc_div(10, 2, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CALC_EINVAL, err);
}

/* =========================================================
 * main
 * ========================================================= */

int main(void)
{
    /* calc_add */
    RUN_TEST(test_add_positive_numbers_returns_sum);
    RUN_TEST(test_add_zero_and_zero_returns_zero);
    RUN_TEST(test_add_negative_and_positive_returns_sum);
    RUN_TEST(test_add_two_negatives_returns_sum);
    RUN_TEST(test_add_int_max_and_zero_returns_int_max);
    RUN_TEST(test_add_int_min_and_zero_returns_int_min);
    RUN_TEST(test_add_null_result_returns_einval);

    /* calc_sub */
    RUN_TEST(test_sub_larger_minus_smaller_returns_diff);
    RUN_TEST(test_sub_zero_minus_zero_returns_zero);
    RUN_TEST(test_sub_smaller_minus_larger_returns_negative);
    RUN_TEST(test_sub_two_negatives_returns_diff);
    RUN_TEST(test_sub_int_min_minus_zero_returns_int_min);
    RUN_TEST(test_sub_zero_minus_int_min_returns_int_min_plus_one);
    RUN_TEST(test_sub_null_result_returns_einval);

    /* calc_mul */
    RUN_TEST(test_mul_two_positives_returns_product);
    RUN_TEST(test_mul_by_zero_returns_zero);
    RUN_TEST(test_mul_negative_and_positive_returns_negative);
    RUN_TEST(test_mul_two_negatives_returns_positive);
    RUN_TEST(test_mul_one_and_int_max_returns_int_max);
    RUN_TEST(test_mul_zero_and_int_min_returns_zero);
    RUN_TEST(test_mul_null_result_returns_einval);

    /* calc_div */
    RUN_TEST(test_div_even_division_returns_quotient);
    RUN_TEST(test_div_truncates_remainder);
    RUN_TEST(test_div_zero_dividend_returns_zero);
    RUN_TEST(test_div_negative_dividend_returns_negative);
    RUN_TEST(test_div_int_min_by_one_returns_int_min);
    RUN_TEST(test_div_same_values_returns_one);
    RUN_TEST(test_div_by_zero_returns_edom);
    RUN_TEST(test_div_null_result_returns_einval);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
