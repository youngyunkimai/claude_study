# Calculator C Library — TDD Test Scenarios

테스트 이름 형식: `test_[단위]_[시나리오]_[기대결과]`

---

## 1. 덧셈 — `calc_add`

### 정상 동작

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 1 | `test_add_positive_numbers_returns_sum` | `1` | `2` | `0` | `3` | 양수 + 양수 |
| 2 | `test_add_zero_and_zero_returns_zero` | `0` | `0` | `0` | `0` | 0 + 0 |
| 3 | `test_add_negative_and_positive_returns_sum` | `-3` | `5` | `0` | `2` | 음수 + 양수 |
| 4 | `test_add_two_negatives_returns_sum` | `-3` | `-4` | `0` | `-7` | 음수 + 음수 |

### 경계 조건

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 5 | `test_add_int_max_and_zero_returns_int_max` | `INT_MAX` | `0` | `0` | `INT_MAX` | 최댓값 + 0 |
| 6 | `test_add_int_min_and_zero_returns_int_min` | `INT_MIN` | `0` | `0` | `INT_MIN` | 최솟값 + 0 |

### 에러 케이스

| # | 테스트 이름 | `a` | `b` | `result` | 기대 반환값 | 설명 |
|---|-------------|-----|-----|----------|-------------|------|
| 7 | `test_add_null_result_returns_einval` | `1` | `2` | `NULL` | `-EINVAL` | NULL 포인터 |

---

## 2. 뺄셈 — `calc_sub`

### 정상 동작

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 1 | `test_sub_larger_minus_smaller_returns_diff` | `5` | `3` | `0` | `2` | 기본 뺄셈 |
| 2 | `test_sub_zero_minus_zero_returns_zero` | `0` | `0` | `0` | `0` | 0 - 0 |
| 3 | `test_sub_smaller_minus_larger_returns_negative` | `3` | `5` | `0` | `-2` | 결과가 음수 |
| 4 | `test_sub_two_negatives_returns_diff` | `-3` | `-4` | `0` | `1` | 음수 - 음수 |

### 경계 조건

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 5 | `test_sub_int_min_minus_zero_returns_int_min` | `INT_MIN` | `0` | `0` | `INT_MIN` | 최솟값 - 0 |
| 6 | `test_sub_zero_minus_int_min_returns_int_min_plus_one` | `0` | `INT_MIN+1` | `0` | `INT_MIN+1` 의 반대 | 최솟값 근접 |

### 에러 케이스

| # | 테스트 이름 | `a` | `b` | `result` | 기대 반환값 | 설명 |
|---|-------------|-----|-----|----------|-------------|------|
| 7 | `test_sub_null_result_returns_einval` | `5` | `3` | `NULL` | `-EINVAL` | NULL 포인터 |

---

## 3. 곱셈 — `calc_mul`

### 정상 동작

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 1 | `test_mul_two_positives_returns_product` | `3` | `4` | `0` | `12` | 양수 × 양수 |
| 2 | `test_mul_by_zero_returns_zero` | `0` | `999` | `0` | `0` | 0 곱하기 |
| 3 | `test_mul_negative_and_positive_returns_negative` | `-3` | `4` | `0` | `-12` | 음수 × 양수 |
| 4 | `test_mul_two_negatives_returns_positive` | `-3` | `-4` | `0` | `12` | 음수 × 음수 |

### 경계 조건

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 5 | `test_mul_one_and_int_max_returns_int_max` | `1` | `INT_MAX` | `0` | `INT_MAX` | 항등원 × 최댓값 |
| 6 | `test_mul_zero_and_int_min_returns_zero` | `0` | `INT_MIN` | `0` | `0` | 0 × 최솟값 |

### 에러 케이스

| # | 테스트 이름 | `a` | `b` | `result` | 기대 반환값 | 설명 |
|---|-------------|-----|-----|----------|-------------|------|
| 7 | `test_mul_null_result_returns_einval` | `3` | `4` | `NULL` | `-EINVAL` | NULL 포인터 |

---

## 4. 나눗셈 — `calc_div`

### 정상 동작

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 1 | `test_div_even_division_returns_quotient` | `10` | `2` | `0` | `5` | 나누어 떨어짐 |
| 2 | `test_div_truncates_remainder` | `7` | `2` | `0` | `3` | 소수점 버림 |
| 3 | `test_div_zero_dividend_returns_zero` | `0` | `5` | `0` | `0` | 0 ÷ 양수 |
| 4 | `test_div_negative_dividend_returns_negative` | `-9` | `3` | `0` | `-3` | 음수 ÷ 양수 |

### 경계 조건

| # | 테스트 이름 | `a` | `b` | 기대 반환값 | 기대 `*result` | 설명 |
|---|-------------|-----|-----|-------------|----------------|------|
| 5 | `test_div_int_min_by_one_returns_int_min` | `INT_MIN` | `1` | `0` | `INT_MIN` | 최솟값 ÷ 1 |
| 6 | `test_div_same_values_returns_one` | `INT_MAX` | `INT_MAX` | `0` | `1` | 같은 값으로 나누기 |

### 에러 케이스

| # | 테스트 이름 | `a` | `b` | `result` | 기대 반환값 | 설명 |
|---|-------------|-----|-----|----------|-------------|------|
| 7 | `test_div_by_zero_returns_edom` | `10` | `0` | (valid) | `-EDOM` | 0으로 나누기 |
| 8 | `test_div_null_result_returns_einval` | `10` | `2` | `NULL` | `-EINVAL` | NULL 포인터 |

---

## 전체 테스트 목록 (29개)

| # | 테스트 이름 | 분류 |
|---|-------------|------|
| 1 | `test_add_positive_numbers_returns_sum` | 정상 |
| 2 | `test_add_zero_and_zero_returns_zero` | 정상 |
| 3 | `test_add_negative_and_positive_returns_sum` | 정상 |
| 4 | `test_add_two_negatives_returns_sum` | 정상 |
| 5 | `test_add_int_max_and_zero_returns_int_max` | 경계 |
| 6 | `test_add_int_min_and_zero_returns_int_min` | 경계 |
| 7 | `test_add_null_result_returns_einval` | 에러 |
| 8 | `test_sub_larger_minus_smaller_returns_diff` | 정상 |
| 9 | `test_sub_zero_minus_zero_returns_zero` | 정상 |
| 10 | `test_sub_smaller_minus_larger_returns_negative` | 정상 |
| 11 | `test_sub_two_negatives_returns_diff` | 정상 |
| 12 | `test_sub_int_min_minus_zero_returns_int_min` | 경계 |
| 13 | `test_sub_zero_minus_int_min_returns_int_min_plus_one` | 경계 |
| 14 | `test_sub_null_result_returns_einval` | 에러 |
| 15 | `test_mul_two_positives_returns_product` | 정상 |
| 16 | `test_mul_by_zero_returns_zero` | 정상 |
| 17 | `test_mul_negative_and_positive_returns_negative` | 정상 |
| 18 | `test_mul_two_negatives_returns_positive` | 정상 |
| 19 | `test_mul_one_and_int_max_returns_int_max` | 경계 |
| 20 | `test_mul_zero_and_int_min_returns_zero` | 경계 |
| 21 | `test_mul_null_result_returns_einval` | 에러 |
| 22 | `test_div_even_division_returns_quotient` | 정상 |
| 23 | `test_div_truncates_remainder` | 정상 |
| 24 | `test_div_zero_dividend_returns_zero` | 정상 |
| 25 | `test_div_negative_dividend_returns_negative` | 정상 |
| 26 | `test_div_int_min_by_one_returns_int_min` | 경계 |
| 27 | `test_div_same_values_returns_one` | 경계 |
| 28 | `test_div_by_zero_returns_edom` | 에러 |
| 29 | `test_div_null_result_returns_einval` | 에러 |

---

## 분류별 집계

| 분류 | 덧셈 | 뺄셈 | 곱셈 | 나눗셈 | 합계 |
|------|------|------|------|--------|------|
| 정상 | 4 | 4 | 4 | 4 | **16** |
| 경계 | 2 | 2 | 2 | 2 | **8** |
| 에러 | 1 | 1 | 1 | 2 | **5** |
| 합계 | **7** | **7** | **7** | **8** | **29** |
