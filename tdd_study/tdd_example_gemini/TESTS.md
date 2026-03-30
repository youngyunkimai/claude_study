# 계산기 C 라이브러리 테스트 시나리오

본 문서는 TDD(Test Driven Development)를 위한 상세 테스트 시나리오를 정의합니다.

## 테스트 분류 및 시나리오 목록

| 분류 | 테스트 이름 | 입력 (a, b) | 기대 반환값 | 기대 결과값 | 시나리오 설명 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **정상 동작** | `test_add_positive_success` | 10, 20 | `0` | `30` | 두 양수의 덧셈 |
| | `test_sub_positive_success` | 50, 20 | `0` | `30` | 두 양수의 뺄셈 |
| | `test_mul_positive_success` | 10, 5 | `0` | `50` | 두 양수의 곱셈 |
| | `test_div_positive_success` | 10, 2 | `0` | `5` | 정수로 나누어떨어지는 나눗셈 |
| | `test_div_remainder_success` | 7, 3 | `0` | `2` | 나머지가 발생하는 나눗셈 (버림 확인) |
| | `test_calc_with_negative_success` | -10, 5 | `0` | (함수별 결과) | 음수가 포함된 연산 정상 동작 확인 |
| **경계 조건** | `test_add_overflow_failure` | `INT_MAX`, 1 | `-34` (`-ERANGE`) | N/A | 최대값 초과 시 오버플로우 처리 |
| | `test_add_underflow_failure` | `INT_MIN`, -1 | `-34` (`-ERANGE`) | N/A | 최소값 미달 시 언더플로우 처리 |
| | `test_sub_underflow_failure` | `INT_MIN`, 1 | `-34` (`-ERANGE`) | N/A | 뺄셈 결과가 `INT_MIN`보다 작을 때 |
| | `test_mul_overflow_failure` | `INT_MAX`, 2 | `-34` (`-ERANGE`) | N/A | 곱셈 결과가 `INT_MAX`를 초과할 때 |
| | `test_div_int_min_overflow_failure` | `INT_MIN`, -1 | `-34` (`-ERANGE`) | N/A | `INT_MIN / -1` (결과가 `INT_MAX` 초과) |
| **에러 케이스** | `test_div_by_zero_failure` | 10, 0 | `-22` (`-EINVAL`) | N/A | **0으로 나누기 시도 시 에러 반환** |
| | `test_common_null_pointer_failure` | 10, 20 | `-22` (`-EINVAL`) | N/A | 결과값 포인터가 `NULL`인 경우 에러 |

## 참고 사항
- **반환값 정의**:
    - `0`: `SUCCESS`
    - `-22`: `EINVAL` (Invalid Argument)
    - `-34`: `ERANGE` (Result out of range)
- **N/A**: 에러 반환 시 결과 포인터(`*result`)의 값은 검증하지 않거나 변경되지 않아야 함을 의미합니다.
