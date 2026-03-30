#ifndef CALCULATOR_H
#define CALCULATOR_H

/**
 * @brief 계산기 에러 코드 정의
 * Linux 커널 스타일을 준수하여 0은 성공, 음수는 에러를 나타냅니다.
 */
typedef enum {
    CALC_OK = 0,
    CALC_DIV_ZERO = -1,
    CALC_ERR_RANGE = -2,   /* 오버플로우/언더플로우 */
    CALC_ERR_INVALID = -3  /* NULL 포인터 등 유효하지 않은 인자 */
} calc_error_t;

/**
 * @brief 정수 덧셈
 * @return CALC_OK(0) 성공, 그 외 에러 코드
 */
int add(int a, int b, int *result);

/**
 * @brief 정수 뺄셈
 * @return CALC_OK(0) 성공, 그 외 에러 코드
 */
int subtract(int a, int b, int *result);

/**
 * @brief 정수 곱셈
 * @return CALC_OK(0) 성공, 그 외 에러 코드
 */
int multiply(int a, int b, int *result);

/**
 * @brief 정수 나눗셈
 * @param result 연산 결과를 저장할 포인터 (나머지는 버림)
 * @return CALC_OK(0) 성공, CALC_DIV_ZERO(-1) 0으로 나누기 에러
 */
int divide(int a, int b, int *result);

#endif /* CALCULATOR_H */
