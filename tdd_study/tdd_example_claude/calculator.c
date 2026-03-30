#include <stddef.h>
#include "calculator.h"

/* result가 NULL이면 연산 자체를 시작할 수 없다.
 * 산술 검사(b == 0 등)보다 먼저 확인해야 역참조 사고를 막는다. */
static inline int result_is_null(const int *result)
{
    return result == NULL;
}

int calc_add(int a, int b, int *result)
{
    if (result_is_null(result))
        return CALC_EINVAL;

    *result = a + b;
    return CALC_OK;
}

int calc_sub(int a, int b, int *result)
{
    if (result_is_null(result))
        return CALC_EINVAL;

    *result = a - b;
    return CALC_OK;
}

int calc_mul(int a, int b, int *result)
{
    if (result_is_null(result))
        return CALC_EINVAL;

    *result = a * b;
    return CALC_OK;
}

int calc_div(int a, int b, int *result)
{
    if (result_is_null(result))
        return CALC_EINVAL;

    /* 수학적으로 정의되지 않은 연산은 결과를 쓰지 않고 즉시 반환한다. */
    if (b == 0)
        return CALC_DIV_ZERO;

    *result = a / b;
    return CALC_OK;
}
