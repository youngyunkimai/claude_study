/*
 * calculator.c — Module 1 구현
 *
 * ============================================================
 * TDD 실습 안내
 * ============================================================
 *
 * 이 파일을 처음 만들 때는 아래처럼 스텁으로 시작합니다:
 *
 *   int add(int a, int b) { return 0; }   ← RED: 0 != 예상값
 *
 * 그런 다음 테스트를 한 번에 하나씩 통과시키면서 채워나갑니다.
 * 현재 파일은 모든 테스트가 통과하는 최종 상태입니다.
 *
 * TDD Law 3: "테스트를 통과할 만큼만 프로덕션 코드를 작성하라"
 * - 테스트에 없는 edge case를 미리 처리하지 마세요.
 * - 테스트가 요구할 때 추가하세요.
 */

#include "calculator.h"

/*
 * add — 두 정수의 합
 *
 * TDD 진행 이력:
 *   RED:    return 0;                     (test: 1+2 == 3 → FAIL)
 *   GREEN:  return a + b;                 (test: 1+2 == 3 → PASS)
 *   REFACTOR: (이 경우 리팩토링할 것 없음)
 */
int add(int a, int b)
{
    return a + b;
}

/*
 * subtract — 두 정수의 차
 */
int subtract(int a, int b)
{
    return a - b;
}

/*
 * multiply — 두 정수의 곱
 */
int multiply(int a, int b)
{
    return a * b;
}

/*
 * divide — 나눗셈 (0으로 나누기 에러 처리)
 *
 * TDD 진행 이력:
 *   RED:    return CALC_OK; (b==0 케이스 없음 → divide-by-zero 테스트 FAIL)
 *   GREEN:  아래 구현 추가 → PASS
 *   REFACTOR: (필요시 에러 처리 패턴 통일)
 *
 * 커널 스타일 포인트:
 *   - NULL 포인터 체크를 먼저
 *   - 에러 조건을 먼저 처리하고 빠르게 리턴 (early return)
 */
calc_err_t divide(int a, int b, int *result)
{
    if (b == 0)
        return CALC_DIV_ZERO;

    *result = a / b;
    return CALC_OK;
}
