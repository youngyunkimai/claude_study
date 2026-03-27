/*
 * test_ring_buffer.c — Module 2: 경계조건과 상태 TDD
 *
 * ============================================================
 * 이 모듈의 테스트 설계 전략
 * ============================================================
 *
 * 테스트는 4가지 영역을 커버합니다:
 *
 *   1. 초기 상태(Initial State)
 *      init 직후 상태가 올바른지
 *
 *   2. 정상 경로(Happy Path)
 *      push, pop이 기대대로 동작하는지
 *
 *   3. 경계조건(Boundary Conditions) ← 버그가 주로 여기에
 *      빈 버퍼 pop, 가득 찬 버퍼 push, capacity=1 케이스
 *
 *   4. Wrap-around ← 원형 버퍼의 핵심 동작
 *      head/tail 인덱스가 capacity를 넘어 0으로 돌아오는 케이스
 *
 * TDD 스킬: 테스트를 이 순서로 작성하면
 *           구현도 이 순서로 복잡해지며 자연스럽게 진화합니다.
 */

#include "../framework/test_framework.h"
#include "ring_buffer.h"

/* ----------------------------------------------------------
 * 헬퍼: 각 테스트에서 재사용할 초기화/해제 패턴
 *
 * TDD FIRST 원칙 중 'I' (Independent):
 * 각 테스트는 독립적으로 실행되어야 합니다.
 * 테스트 간에 전역 상태를 공유하면 순서 의존성이 생깁니다.
 * 여기서는 각 테스트가 자체 ring_buffer_t를 스택에 선언합니다.
 * ---------------------------------------------------------- */

/* ----------------------------------------------------------
 * 1. 초기 상태 테스트
 * ---------------------------------------------------------- */

void test_rb_init_creates_empty_buffer(void)
{
    ring_buffer_t rb;
    int ret = rb_init(&rb, 4);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT(rb_is_empty(&rb));
    TEST_ASSERT(!rb_is_full(&rb));
    TEST_ASSERT_EQUAL_SIZE(0, rb_count(&rb));

    rb_destroy(&rb);
}

void test_rb_init_with_zero_capacity_returns_error(void)
{
    ring_buffer_t rb;
    int ret = rb_init(&rb, 0);

    /* capacity 0은 무의미 → 에러 반환 */
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

/* ----------------------------------------------------------
 * 2. 정상 경로 테스트
 * ---------------------------------------------------------- */

void test_rb_push_one_item_count_becomes_one(void)
{
    ring_buffer_t rb;
    rb_init(&rb, 4);

    int ret = rb_push(&rb, 0xAB);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_SIZE(1, rb_count(&rb));
    TEST_ASSERT(!rb_is_empty(&rb));

    rb_destroy(&rb);
}

void test_rb_pop_returns_pushed_value(void)
{
    ring_buffer_t rb;
    rb_init(&rb, 4);
    rb_push(&rb, 0x42);

    uint8_t val = 0;
    int ret = rb_pop(&rb, &val);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0x42, val);
    TEST_ASSERT(rb_is_empty(&rb));

    rb_destroy(&rb);
}

void test_rb_fifo_order_is_preserved(void)
{
    /*
     * FIFO 순서 검증: 먼저 넣은 것이 먼저 나와야 합니다.
     * push: 1, 2, 3
     * pop:  1, 2, 3 (FIFO)
     */
    ring_buffer_t rb;
    rb_init(&rb, 4);

    rb_push(&rb, 1);
    rb_push(&rb, 2);
    rb_push(&rb, 3);

    uint8_t val;
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(1, val);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(2, val);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(3, val);

    rb_destroy(&rb);
}

/* ----------------------------------------------------------
 * 3. 경계조건 테스트 ← 여기서 버그가 발견됩니다
 * ---------------------------------------------------------- */

void test_rb_push_when_full_returns_error(void)
{
    ring_buffer_t rb;
    rb_init(&rb, 2);

    rb_push(&rb, 1);
    rb_push(&rb, 2);

    /* 가득 찬 상태 확인 */
    TEST_ASSERT(rb_is_full(&rb));

    /* 추가 push는 실패해야 함 */
    int ret = rb_push(&rb, 3);
    TEST_ASSERT_EQUAL_INT(-1, ret);

    /* 실패한 push가 버퍼를 손상시키지 않아야 함 */
    TEST_ASSERT_EQUAL_SIZE(2, rb_count(&rb));

    rb_destroy(&rb);
}

void test_rb_pop_when_empty_returns_error(void)
{
    ring_buffer_t rb;
    rb_init(&rb, 4);

    /* 빈 버퍼에서 pop */
    uint8_t val = 0xFF;
    int ret = rb_pop(&rb, &val);

    TEST_ASSERT_EQUAL_INT(-1, ret);
    /* val은 변경되지 않아야 함 */
    TEST_ASSERT_EQUAL_INT(0xFF, val);

    rb_destroy(&rb);
}

void test_rb_capacity_one_push_and_pop(void)
{
    /*
     * capacity=1 경계값 테스트
     * push → full 상태 → pop → empty 상태 사이클
     */
    ring_buffer_t rb;
    rb_init(&rb, 1);

    TEST_ASSERT(rb_is_empty(&rb));

    rb_push(&rb, 0x55);
    TEST_ASSERT(rb_is_full(&rb));
    TEST_ASSERT(!rb_is_empty(&rb));

    uint8_t val;
    rb_pop(&rb, &val);
    TEST_ASSERT_EQUAL_INT(0x55, val);
    TEST_ASSERT(rb_is_empty(&rb));
    TEST_ASSERT(!rb_is_full(&rb));

    rb_destroy(&rb);
}

/* ----------------------------------------------------------
 * 4. Wrap-around 테스트 ← 원형 버퍼의 핵심
 *
 * head/tail 인덱스가 capacity를 넘어 0으로 돌아오는 케이스.
 * 이 테스트 없이는 wrap-around 버그를 잡을 수 없습니다.
 * ---------------------------------------------------------- */

void test_rb_wraparound_head_index(void)
{
    /*
     * capacity=3 버퍼에서 wrap-around 검증:
     *
     * 초기: head=0, tail=0, count=0
     * push A: head=1, tail=0, count=1  buf=[A, _, _]
     * push B: head=2, tail=0, count=2  buf=[A, B, _]
     * push C: head=0, tail=0, count=3  buf=[A, B, C]  ← head wrap!
     * pop  →: tail=1, count=2 → returns A
     * pop  →: tail=2, count=1 → returns B
     * push D: head=1, count=2           buf=[D, B, C]  ← head가 0 다음 1
     * pop  →: tail=0, count=1 → returns C
     * pop  →: tail=1, count=0 → returns D
     */
    ring_buffer_t rb;
    rb_init(&rb, 3);

    uint8_t val;

    rb_push(&rb, 'A');
    rb_push(&rb, 'B');
    rb_push(&rb, 'C');   /* head가 0으로 wrap */

    rb_pop(&rb, &val);   TEST_ASSERT_EQUAL_INT('A', val);
    rb_pop(&rb, &val);   TEST_ASSERT_EQUAL_INT('B', val);

    rb_push(&rb, 'D');   /* head=1 위치에 쓰기 */

    rb_pop(&rb, &val);   TEST_ASSERT_EQUAL_INT('C', val);
    rb_pop(&rb, &val);   TEST_ASSERT_EQUAL_INT('D', val);

    TEST_ASSERT(rb_is_empty(&rb));

    rb_destroy(&rb);
}

void test_rb_multiple_push_pop_cycles(void)
{
    /*
     * 여러 사이클 반복 — head/tail 모두 여러 번 wrap-around
     * FIFO 순서가 항상 유지되는지 검증
     */
    ring_buffer_t rb;
    rb_init(&rb, 2);

    uint8_t val;

    /* 사이클 1 */
    rb_push(&rb, 10);
    rb_push(&rb, 20);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(10, val);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(20, val);

    /* 사이클 2 */
    rb_push(&rb, 30);
    rb_push(&rb, 40);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(30, val);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(40, val);

    /* 사이클 3 */
    rb_push(&rb, 50);
    rb_push(&rb, 60);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(50, val);
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_INT(60, val);

    TEST_ASSERT(rb_is_empty(&rb));

    rb_destroy(&rb);
}

void test_rb_count_tracks_correctly_during_mixed_ops(void)
{
    /*
     * count가 혼합 push/pop 동작에서 정확히 추적되는지 검증
     * 경계값: count가 0과 capacity 사이를 정확히 왕복해야 함
     */
    ring_buffer_t rb;
    rb_init(&rb, 4);
    uint8_t val;

    TEST_ASSERT_EQUAL_SIZE(0, rb_count(&rb));

    rb_push(&rb, 1); TEST_ASSERT_EQUAL_SIZE(1, rb_count(&rb));
    rb_push(&rb, 2); TEST_ASSERT_EQUAL_SIZE(2, rb_count(&rb));
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_SIZE(1, rb_count(&rb));
    rb_push(&rb, 3); TEST_ASSERT_EQUAL_SIZE(2, rb_count(&rb));
    rb_push(&rb, 4); TEST_ASSERT_EQUAL_SIZE(3, rb_count(&rb));
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_SIZE(2, rb_count(&rb));
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_SIZE(1, rb_count(&rb));
    rb_pop(&rb, &val); TEST_ASSERT_EQUAL_SIZE(0, rb_count(&rb));

    rb_destroy(&rb);
}

/* ----------------------------------------------------------
 * main
 * ---------------------------------------------------------- */
int main(void)
{
    printf("Module 2: Ring Buffer — 경계조건과 상태 TDD\n");
    printf("------------------------------------------------------\n");

    /* 1. 초기 상태 */
    RUN_TEST(test_rb_init_creates_empty_buffer);
    RUN_TEST(test_rb_init_with_zero_capacity_returns_error);

    /* 2. 정상 경로 */
    RUN_TEST(test_rb_push_one_item_count_becomes_one);
    RUN_TEST(test_rb_pop_returns_pushed_value);
    RUN_TEST(test_rb_fifo_order_is_preserved);

    /* 3. 경계조건 */
    RUN_TEST(test_rb_push_when_full_returns_error);
    RUN_TEST(test_rb_pop_when_empty_returns_error);
    RUN_TEST(test_rb_capacity_one_push_and_pop);

    /* 4. Wrap-around */
    RUN_TEST(test_rb_wraparound_head_index);
    RUN_TEST(test_rb_multiple_push_pop_cycles);
    RUN_TEST(test_rb_count_tracks_correctly_during_mixed_ops);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
