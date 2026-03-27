/*
 * ring_buffer.h — Module 2: 경계조건과 상태 TDD
 *
 * ============================================================
 * 이 모듈에서 배우는 것
 * ============================================================
 *   - 경계조건(Boundary Condition) 테스트
 *   - 상태(State)에 따른 행동 검증
 *   - 테스트가 API 설계를 주도하는 방식 (Test-Driven Design)
 *
 * ============================================================
 * 커널 연관성
 * ============================================================
 * Ring buffer는 커널 곳곳에 사용됩니다:
 *   - kfifo (include/linux/kfifo.h): 동기화된 FIFO 큐
 *   - 네트워크 패킷 큐 (skb ring)
 *   - DMA 버퍼 관리
 *
 * ============================================================
 * TDD 스킬: 경계조건 테스트
 * ============================================================
 * 버그는 경계에서 발생합니다:
 *   - 빈 버퍼에서 pop
 *   - 가득 찬 버퍼에서 push
 *   - 마지막 원소 pop 후 empty 상태
 *   - wrap-around (head/tail 인덱스가 0으로 돌아갈 때)
 *
 * ============================================================
 * 실습 방법
 * ============================================================
 * 1. ring_buffer.c의 모든 함수를 스텁으로 비워두세요.
 * 2. $ make test → RED (테스트 실패 확인)
 * 3. 테스트를 하나씩 통과시키며 구현을 채워나가세요.
 * 4. wrap-around 테스트가 핵심입니다! (head/tail % capacity)
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>

/*
 * ring_buffer_t — 원형 버퍼 (고정 크기)
 *
 * 구조:
 *   buf[0]  buf[1]  buf[2]  ...  buf[capacity-1]
 *              ↑tail                  ↑head
 *   tail: 다음 읽기 위치 (pop)
 *   head: 다음 쓰기 위치 (push)
 *   count: 현재 저장된 원소 수
 *
 * head == tail일 때:
 *   count == 0 → empty
 *   count == capacity → full
 *   → count 필드로 empty/full 구분 (모호함 해소)
 */
typedef struct {
    uint8_t *buf;
    size_t   capacity;
    size_t   head;    /* 다음 push 위치 */
    size_t   tail;    /* 다음 pop 위치 */
    size_t   count;   /* 현재 원소 수 */
} ring_buffer_t;

/*
 * rb_init — 버퍼 초기화 (동적 메모리 할당)
 *
 * @return  0 on success, -1 on failure (capacity==0 or malloc fail)
 */
int rb_init(ring_buffer_t *rb, size_t capacity);

/*
 * rb_destroy — 버퍼 해제
 */
void rb_destroy(ring_buffer_t *rb);

/*
 * rb_push — 데이터 추가 (rear에 삽입)
 *
 * @return  0 on success, -1 if buffer is full
 */
int rb_push(ring_buffer_t *rb, uint8_t data);

/*
 * rb_pop — 데이터 읽기 및 제거 (front에서 추출)
 *
 * @param data  읽은 값을 저장할 포인터
 * @return      0 on success, -1 if buffer is empty
 */
int rb_pop(ring_buffer_t *rb, uint8_t *data);

/*
 * rb_is_empty — 버퍼가 비어있는지 확인
 * rb_is_full  — 버퍼가 가득 찼는지 확인
 * rb_count    — 현재 저장된 원소 수 반환
 */
int    rb_is_empty(const ring_buffer_t *rb);
int    rb_is_full(const ring_buffer_t *rb);
size_t rb_count(const ring_buffer_t *rb);

#endif /* RING_BUFFER_H */
