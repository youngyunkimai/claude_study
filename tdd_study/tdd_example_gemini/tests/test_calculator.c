#include <limits.h>
#include <stddef.h>
#include "../framework/test_framework.h"
#include "../calculator.h"

/* --- 정상 동작 테스트 --- */

void test_add_positive_success() {
    // Arrange
    int a = 10, b = 20, result = 0;
    // Act
    int err = add(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(30, result);
}

void test_sub_positive_success() {
    // Arrange
    int a = 50, b = 20, result = 0;
    // Act
    int err = subtract(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(30, result);
}

void test_mul_positive_success() {
    // Arrange
    int a = 10, b = 5, result = 0;
    // Act
    int err = multiply(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(50, result);
}

void test_div_positive_success() {
    // Arrange
    int a = 10, b = 2, result = 0;
    // Act
    int err = divide(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void test_div_remainder_success() {
    // Arrange
    int a = 7, b = 3, result = 0;
    // Act
    int err = divide(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(2, result);
}

void test_calc_with_negative_success() {
    // Arrange
    int a = -10, b = 5, result = 0;
    // Act & Assert (덧셈 대표)
    int err = add(a, b, &result);
    TEST_ASSERT_EQUAL_INT(CALC_OK, err);
    TEST_ASSERT_EQUAL_INT(-5, result);
}

/* --- 경계 조건 테스트 --- */

void test_add_overflow_failure() {
    // Arrange
    int a = INT_MAX, b = 1, result = 0;
    // Act
    int err = add(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_RANGE, err);
}

void test_add_underflow_failure() {
    // Arrange
    int a = INT_MIN, b = -1, result = 0;
    // Act
    int err = add(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_RANGE, err);
}

void test_sub_underflow_failure() {
    // Arrange
    int a = INT_MIN, b = 1, result = 0;
    // Act
    int err = subtract(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_RANGE, err);
}

void test_mul_overflow_failure() {
    // Arrange
    int a = INT_MAX, b = 2, result = 0;
    // Act
    int err = multiply(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_RANGE, err);
}

void test_div_int_min_overflow_failure() {
    // Arrange
    int a = INT_MIN, b = -1, result = 0;
    // Act
    int err = divide(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_RANGE, err);
}

/* --- 에러 케이스 테스트 --- */

void test_div_by_zero_failure() {
    // Arrange
    int a = 10, b = 0, result = 0;
    // Act
    int err = divide(a, b, &result);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_DIV_ZERO, err);
}

void test_common_null_pointer_failure() {
    // Arrange
    int a = 10, b = 20;
    // Act
    int err = add(a, b, NULL);
    // Assert
    TEST_ASSERT_EQUAL_INT(CALC_ERR_INVALID, err);
}

int main() {
    // 정상 동작
    RUN_TEST(test_add_positive_success);
    RUN_TEST(test_sub_positive_success);
    RUN_TEST(test_mul_positive_success);
    RUN_TEST(test_div_positive_success);
    RUN_TEST(test_div_remainder_success);
    RUN_TEST(test_calc_with_negative_success);

    // 경계 조건
    RUN_TEST(test_add_overflow_failure);
    RUN_TEST(test_add_underflow_failure);
    RUN_TEST(test_sub_underflow_failure);
    RUN_TEST(test_mul_overflow_failure);
    RUN_TEST(test_div_int_min_overflow_failure);

    // 에러 케이스
    RUN_TEST(test_div_by_zero_failure);
    RUN_TEST(test_common_null_pointer_failure);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
