/*
 * test_mem_pool.c — Module 3: 복잡한 상태 관리 TDD
 *
 * ============================================================
 * 이 파일의 역할
 * ============================================================
 * 테스트는 mem_pool의 내부 구현(비트맵? 배열?)을 모릅니다.
 * 오직 외부 동작(API 계약)만 검증합니다.
 *
 * 이것이 Refactoring을 안전하게 만드는 이유입니다:
 *   → 내부를 바꿔도 테스트가 통과하면 외부 동작은 동일합니다.
 *
 * ============================================================
 * 테스트 설계 전략
 * ============================================================
 *   1. 초기 상태 검증
 *   2. 기본 alloc/free 사이클
 *   3. 풀 소진(exhaustion) 경계조건
 *   4. free 후 재사용 검증
 *   5. 방어적 동작 (invalid free, double-free)
 */

#include "../framework/test_framework.h"
#include "mem_pool.h"

/* 테스트에서 사용할 작은 블록 크기 */
#define BLOCK_SIZE  8
#define NUM_BLOCKS  4

/* ----------------------------------------------------------
 * 1. 초기 상태
 * ---------------------------------------------------------- */

void test_pool_init_available_equals_num_blocks(void)
{
    mem_pool_t pool;
    int ret = pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS, pool_available(&pool));

    pool_destroy(&pool);
}

void test_pool_init_with_zero_block_size_returns_error(void)
{
    mem_pool_t pool;
    int ret = pool_init(&pool, 0, NUM_BLOCKS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_pool_init_with_zero_num_blocks_returns_error(void)
{
    mem_pool_t pool;
    int ret = pool_init(&pool, BLOCK_SIZE, 0);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

/* ----------------------------------------------------------
 * 2. 기본 alloc/free 사이클
 * ---------------------------------------------------------- */

void test_pool_alloc_returns_non_null(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    void *ptr = pool_alloc(&pool);
    TEST_ASSERT_NOT_NULL(ptr);

    pool_destroy(&pool);
}

void test_pool_alloc_decreases_available(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    pool_alloc(&pool);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS - 1, pool_available(&pool));

    pool_alloc(&pool);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS - 2, pool_available(&pool));

    pool_destroy(&pool);
}

void test_pool_free_increases_available(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    void *ptr = pool_alloc(&pool);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS - 1, pool_available(&pool));

    pool_free(&pool, ptr);
    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS, pool_available(&pool));

    pool_destroy(&pool);
}

/* ----------------------------------------------------------
 * 3. 풀 소진 경계조건
 * ---------------------------------------------------------- */

void test_pool_alloc_when_exhausted_returns_null(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    /* 모든 블록 할당 */
    for (int i = 0; i < NUM_BLOCKS; i++)
        pool_alloc(&pool);

    TEST_ASSERT_EQUAL_INT(0, pool_available(&pool));

    /* 추가 alloc은 NULL 반환 */
    void *ptr = pool_alloc(&pool);
    TEST_ASSERT_NULL(ptr);

    pool_destroy(&pool);
}

void test_pool_alloc_all_then_free_all_restores_available(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    void *ptrs[NUM_BLOCKS];

    /* 전부 할당 */
    for (int i = 0; i < NUM_BLOCKS; i++) {
        ptrs[i] = pool_alloc(&pool);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }
    TEST_ASSERT_EQUAL_INT(0, pool_available(&pool));

    /* 전부 반환 */
    for (int i = 0; i < NUM_BLOCKS; i++)
        pool_free(&pool, ptrs[i]);

    TEST_ASSERT_EQUAL_INT(NUM_BLOCKS, pool_available(&pool));

    pool_destroy(&pool);
}

/* ----------------------------------------------------------
 * 4. 해제된 블록 재사용 검증
 *
 * TDD 스킬: "해제 후 재사용"은 메모리 풀의 핵심 동작입니다.
 * 이 테스트 없이는 free가 실제로 블록을 반환하는지 확인할 수 없습니다.
 * ---------------------------------------------------------- */

void test_pool_freed_block_can_be_reallocated(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    /* 모든 블록 할당 */
    void *ptrs[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++)
        ptrs[i] = pool_alloc(&pool);

    /* 하나 해제 */
    pool_free(&pool, ptrs[0]);
    TEST_ASSERT_EQUAL_INT(1, pool_available(&pool));

    /* 다시 할당 가능 */
    void *new_ptr = pool_alloc(&pool);
    TEST_ASSERT_NOT_NULL(new_ptr);
    TEST_ASSERT_EQUAL_INT(0, pool_available(&pool));

    pool_destroy(&pool);
}

/* ----------------------------------------------------------
 * 5. 방어적 동작
 *
 * TDD 스킬: 커널 코드에서 잘못된 입력에 대한 방어는 필수입니다.
 * 이 테스트들은 구현이 gracefully하게 실패하는지 검증합니다.
 * crash나 undefined behavior가 없어야 합니다.
 * ---------------------------------------------------------- */

void test_pool_free_invalid_pointer_is_ignored(void)
{
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    int before = pool_available(&pool);

    /* 풀 범위 밖의 포인터 — 무시해야 함 */
    int dummy = 0;
    pool_free(&pool, &dummy);  /* 풀 범위 밖 */

    /* available이 변하지 않아야 함 */
    TEST_ASSERT_EQUAL_INT(before, pool_available(&pool));

    pool_destroy(&pool);
}

void test_pool_double_free_is_ignored(void)
{
    /*
     * double-free: 같은 포인터를 두 번 해제
     * 두 번째 free는 무시되어야 합니다.
     * available이 num_blocks를 초과하면 안 됩니다.
     */
    mem_pool_t pool;
    pool_init(&pool, BLOCK_SIZE, NUM_BLOCKS);

    void *ptr = pool_alloc(&pool);
    pool_free(&pool, ptr);

    int after_first_free = pool_available(&pool);

    /* 두 번째 free — 무시되어야 함 */
    pool_free(&pool, ptr);

    TEST_ASSERT_EQUAL_INT(after_first_free, pool_available(&pool));

    pool_destroy(&pool);
}

/* ----------------------------------------------------------
 * main
 * ---------------------------------------------------------- */
int main(void)
{
    printf("Module 3: Memory Pool — 복잡한 상태 관리 TDD\n");
    printf("------------------------------------------------------\n");
    printf("Refactoring 실습:\n");
    printf("  1. mem_pool.c를 배열 기반 free list로 교체\n");
    printf("  2. make test → 모든 테스트가 여전히 GREEN이어야 함\n");
    printf("------------------------------------------------------\n");

    /* 1. 초기 상태 */
    RUN_TEST(test_pool_init_available_equals_num_blocks);
    RUN_TEST(test_pool_init_with_zero_block_size_returns_error);
    RUN_TEST(test_pool_init_with_zero_num_blocks_returns_error);

    /* 2. 기본 사이클 */
    RUN_TEST(test_pool_alloc_returns_non_null);
    RUN_TEST(test_pool_alloc_decreases_available);
    RUN_TEST(test_pool_free_increases_available);

    /* 3. 소진 경계조건 */
    RUN_TEST(test_pool_alloc_when_exhausted_returns_null);
    RUN_TEST(test_pool_alloc_all_then_free_all_restores_available);

    /* 4. 재사용 */
    RUN_TEST(test_pool_freed_block_can_be_reallocated);

    /* 5. 방어적 동작 */
    RUN_TEST(test_pool_free_invalid_pointer_is_ignored);
    RUN_TEST(test_pool_double_free_is_ignored);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
