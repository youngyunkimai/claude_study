#ifndef CALCULATOR_H
#define CALCULATOR_H

enum calc_error {
    CALC_OK       =  0,
    CALC_DIV_ZERO = -1,
    CALC_EINVAL   = -2,
};

int calc_add(int a, int b, int *result);
int calc_sub(int a, int b, int *result);
int calc_mul(int a, int b, int *result);
int calc_div(int a, int b, int *result);

#endif /* CALCULATOR_H */
