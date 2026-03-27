/*
 * ring_buffer.c — Module 2 구현
 *
 * TDD 실습을 위해 각 함수에 구현 이전의 스텁 상태를 주석으로 표시했습니다.
 * 직접 실습할 때는:
 *   1. 모든 함수 body를 주석 처리하거나 스텁으로 변경
 *   2. make test로 RED 상태 확인
 *   3. 테스트 하나씩 통과시키며 구현 추가
 */

#include "ring_buffer.h"
#include <stdlib.h>

/*
 * rb_init
 *
 * TDD 진행:
 *   스텁: return 0;
 *   → test_rb_init_creates_empty_buffer: rb_is_empty() 반환값 틀림 → RED
 *   → 실제 구현 추가 → GREEN
 */
int rb_init(ring_buffer_t *rb, size_t capacity)
{
    if (!rb || capacity == 0)
        return -1;

    rb->buf = malloc(capacity);
    if (!rb->buf)
        return -1;

    rb->capacity = capacity;
    rb->head     = 0;
    rb->tail     = 0;
    rb->count    = 0;
    return 0;
}

void rb_destroy(ring_buffer_t *rb)
{
    if (rb && rb->buf) {
        free(rb->buf);
        rb->buf = NULL;
    }
}

/*
 * rb_push
 *
 * 핵심: head = (head + 1) % capacity → wrap-around!
 * 이 한 줄이 "원형" 버퍼를 만드는 핵심입니다.
 *
 * 스텁으로 시작할 때:
 *   return 0;  ← full 체크 없음, head 증가 없음 → 테스트 실패
 */
int rb_push(ring_buffer_t *rb, uint8_t data)
{
    if (!rb || rb_is_full(rb))
        return -1;

    rb->buf[rb->head] = data;
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count++;
    return 0;
}

/*
 * rb_pop
 *
 * 핵심: tail = (tail + 1) % capacity → wrap-around!
 */
int rb_pop(ring_buffer_t *rb, uint8_t *data)
{
    if (!rb || !data || rb_is_empty(rb))
        return -1;

    *data = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count--;
    return 0;
}

int rb_is_empty(const ring_buffer_t *rb)
{
    return rb->count == 0;
}

int rb_is_full(const ring_buffer_t *rb)
{
    return rb->count == rb->capacity;
}

size_t rb_count(const ring_buffer_t *rb)
{
    return rb->count;
}
