/*
 * mem_pool.c — Module 3 구현 (비트맵 기반 free list)
 *
 * ============================================================
 * Refactoring 실습용 주석
 * ============================================================
 * 현재 구현: 비트맵 기반
 *   - bitmap의 각 비트가 블록 하나를 나타냄
 *   - 1 = free, 0 = used
 *   - alloc: 첫 번째 1 비트 찾기 → 0으로 마킹
 *   - free:  해당 비트를 1로 복원
 *
 * 대안 구현: 배열 기반 free list (직접 작성해보세요)
 *   int free_stack[MAX_BLOCKS];
 *   int stack_top;
 *   - alloc: free_stack[stack_top--] 반환
 *   - free:  free_stack[++stack_top] = index
 *
 * 두 구현 모두 test_mem_pool.c의 모든 테스트를 통과해야 합니다.
 */

#include "mem_pool.h"
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------
 * 비트맵 헬퍼 (내부 함수)
 * ---------------------------------------------------------- */

/* 비트 idx를 1로 설정 (free 표시) */
static void bitmap_set(unsigned char *bitmap, size_t idx)
{
    bitmap[idx / 8] |= (1u << (idx % 8));
}

/* 비트 idx를 0으로 설정 (used 표시) */
static void bitmap_clear(unsigned char *bitmap, size_t idx)
{
    bitmap[idx / 8] &= ~(1u << (idx % 8));
}

/* 비트 idx가 1인지 확인 (free 여부) */
static int bitmap_test(const unsigned char *bitmap, size_t idx)
{
    return (bitmap[idx / 8] >> (idx % 8)) & 1;
}

/* 첫 번째 free 블록(비트=1)의 인덱스 반환, 없으면 -1 */
static int bitmap_find_first_free(const unsigned char *bitmap, size_t num_blocks)
{
    for (size_t i = 0; i < num_blocks; i++) {
        if (bitmap_test(bitmap, i))
            return (int)i;
    }
    return -1;
}

/* ----------------------------------------------------------
 * 공개 API 구현
 * ---------------------------------------------------------- */

int pool_init(mem_pool_t *pool, size_t block_size, size_t num_blocks)
{
    if (!pool || block_size == 0 || num_blocks == 0)
        return -1;
    if (num_blocks > MEM_POOL_MAX_BLOCKS)
        return -1;

    pool->memory = malloc(block_size * num_blocks);
    if (!pool->memory)
        return -1;

    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->available  = (int)num_blocks;

    /* 모든 비트를 1로 초기화 (모든 블록 free) */
    memset(pool->bitmap, 0xFF, sizeof(pool->bitmap));

    return 0;
}

void *pool_alloc(mem_pool_t *pool)
{
    if (!pool || pool->available == 0)
        return NULL;

    int idx = bitmap_find_first_free(pool->bitmap, pool->num_blocks);
    if (idx < 0)
        return NULL;

    bitmap_clear(pool->bitmap, (size_t)idx);
    pool->available--;

    return (char *)pool->memory + (size_t)idx * pool->block_size;
}

void pool_free(mem_pool_t *pool, void *ptr)
{
    if (!pool || !ptr || !pool->memory)
        return;

    /* 포인터가 풀 범위 내에 있는지 확인 */
    char *base = (char *)pool->memory;
    char *end  = base + pool->num_blocks * pool->block_size;
    char *p    = (char *)ptr;

    if (p < base || p >= end)
        return;  /* 유효하지 않은 포인터: 무시 */

    /* 블록 경계에 정렬된 포인터인지 확인 */
    size_t offset = (size_t)(p - base);
    if (offset % pool->block_size != 0)
        return;  /* 잘못된 정렬: 무시 */

    size_t idx = offset / pool->block_size;

    /* double-free 방어: 이미 free된 블록은 무시 */
    if (bitmap_test(pool->bitmap, idx))
        return;

    bitmap_set(pool->bitmap, idx);
    pool->available++;
}

int pool_available(const mem_pool_t *pool)
{
    if (!pool)
        return 0;
    return pool->available;
}

void pool_destroy(mem_pool_t *pool)
{
    if (pool && pool->memory) {
        free(pool->memory);
        pool->memory = NULL;
    }
}
