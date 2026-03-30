#include <stddef.h>
#include <limits.h>
#include "calculator.h"

/*
 * Linux kernel coding style uses 8-character tabs.
 * This file follows the style for brace placement and early returns.
 */

int add(int a, int b, int *result)
{
	if (!result)
		return CALC_ERR_INVALID;

	/* Check for overflow or underflow before performing operation */
	if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
		return CALC_ERR_RANGE;

	*result = a + b;
	return CALC_OK;
}

int subtract(int a, int b, int *result)
{
	if (!result)
		return CALC_ERR_INVALID;

	/* Check for overflow or underflow by reversing the operation */
	if ((b > 0 && a < INT_MIN + b) || (b < 0 && a > INT_MAX + b))
		return CALC_ERR_RANGE;

	*result = a - b;
	return CALC_OK;
}

int multiply(int a, int b, int *result)
{
	if (!result)
		return CALC_ERR_INVALID;

	/* 
	 * Multiplication overflow detection requires checking bounds 
	 * by dividing limits by one operand.
	 */
	if (a > 0) {
		if (b > 0 && a > INT_MAX / b)
			return CALC_ERR_RANGE;
		if (b < 0 && b < INT_MIN / a)
			return CALC_ERR_RANGE;
	} else if (a < 0) {
		if (b > 0 && a < INT_MIN / b)
			return CALC_ERR_RANGE;
		if (b < 0 && b < INT_MAX / a)
			return CALC_ERR_RANGE;
	}

	*result = a * b;
	return CALC_OK;
}

int divide(int a, int b, int *result)
{
	if (!result)
		return CALC_ERR_INVALID;

	if (b == 0)
		return CALC_DIV_ZERO;

	/* 
	 * INT_MIN divided by -1 results in INT_MAX + 1, 
	 * which causes positive overflow in two's complement.
	 */
	if (a == INT_MIN && b == -1)
		return CALC_ERR_RANGE;

	*result = a / b;
	return CALC_OK;
}
