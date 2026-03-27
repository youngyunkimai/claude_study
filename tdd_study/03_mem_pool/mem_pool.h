/*
 * mem_pool.h — Module 3: 복잡한 상태 관리 TDD + Refactoring 실습
 *
 * ============================================================
 * 이 모듈에서 배우는 것
 * ============================================================
 *   - 복잡한 내부 상태를 가진 모듈 TDD
 *   - Refactoring: 테스트를 유지하며 내부 구현 교체
 *   - double-free, 유효하지 않은 포인터 방어
 *
 * ============================================================
 * 커널 연관성
 * ============================================================
 * Linux 커널의 slab/slub allocator의 단순화 버전입니다:
 *   - 고정 크기 블록 (slab의 object)
 *   - 미리 할당된 메모리 풀 (kmem_cache)
 *   - 빠른 alloc/free (O(1))
 *
 * 실제 커널 코드: mm/slab.c, mm/slub.c
 *
 * ============================================================
 * Refactoring 실습 (Module 3의 핵심)
 * ============================================================
 * 구현을 두 가지 방식으로 작성하고 교체해보세요:
 *
 * 방식 A (배열 기반 free list):
 *   int free_list[NUM_BLOCKS];  ← 빈 블록 인덱스 스택
 *   pop으로 alloc, push로 free
 *
 * 방식 B (비트맵 기반):
 *   unsigned char bitmap[NUM_BLOCKS/8 + 1];  ← 비트가 1이면 free
 *   선형 탐색으로 빈 슬롯 찾기
 *
 * 두 방식 모두 동일한 테스트를 통과해야 합니다.
 * 이것이 리팩토링의 핵심: 외부 동작(테스트)은 변하지 않고 내부만 변경.
 *
 * ============================================================
 * 실습 순서
 * ============================================================
 * 1. 방식 A로 구현 → make test → 모두 GREEN
 * 2. mem_pool.c를 방식 B로 완전히 교체
 * 3. make test → 여전히 모두 GREEN
 *    → "내부 구현이 달라도 테스트가 보장하는 동작은 변하지 않는다"
 */

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stddef.h>

/* 풀이 관리할 수 있는 최대 블록 수 (비트맵 크기 계산용) */
#define MEM_POOL_MAX_BLOCKS 64

/*
 * mem_pool_t — 고정 크기 블록 메모리 풀
 *
 * 내부 구현은 모듈이 숨깁니다 (encapsulation).
 * 사용자는 API만 알면 됩니다.
 *
 * 현재 구현: 비트맵 기반 free list
 *   bitmap[i] 비트 k가 1이면 블록 (i*8+k)는 free
 */
typedef struct {
    void  *memory;              /* 실제 데이터를 저장하는 backing store */
    size_t block_size;          /* 각 블록의 크기 (바이트) */
    size_t num_blocks;          /* 총 블록 수 */
    int    available;           /* 현재 사용 가능한 블록 수 */
    /* 비트맵: 1=free, 0=used. 최대 MEM_POOL_MAX_BLOCKS 블록 지원 */
    unsigned char bitmap[(MEM_POOL_MAX_BLOCKS + 7) / 8];
} mem_pool_t;

/*
 * pool_init — 메모리 풀 초기화
 *
 * @param block_size  각 블록의 크기 (>0)
 * @param num_blocks  총 블록 수 (>0, <= MEM_POOL_MAX_BLOCKS)
 * @return  0 on success, -1 on error
 */
int pool_init(mem_pool_t *pool, size_t block_size, size_t num_blocks);

/*
 * pool_alloc — 블록 한 개 할당
 *
 * @return  블록 포인터, 풀이 소진되면 NULL
 */
void *pool_alloc(mem_pool_t *pool);

/*
 * pool_free — 블록 반환
 *
 * 유효하지 않은 포인터(범위 밖)는 무시합니다.
 * double-free도 안전하게 무시합니다.
 */
void pool_free(mem_pool_t *pool, void *ptr);

/*
 * pool_available — 남은 블록 수 반환
 */
int pool_available(const mem_pool_t *pool);

/*
 * pool_destroy — 풀 해제
 */
void pool_destroy(mem_pool_t *pool);

#endif /* MEM_POOL_H */
