/*
 * led_fsm.h — Module 4: LED 상태 머신
 *
 * ============================================================
 * 상태 머신 설계
 * ============================================================
 *
 *         led_fsm_set(LED_ON)        led_fsm_set(LED_OFF)
 *   ┌──────────────────────┐  ┌──────────────────────┐
 *   │                      ▼  │                      │
 * [OFF] ──────────────── [ON]  [BLINK] ─────────────[OFF]
 *   ▲   led_fsm_set(LED_OFF)     │
 *   │                           │ led_fsm_set(LED_BLINK)
 *   └───────────────────────────┘
 *
 * LED_BLINK 상태에서는 led_fsm_tick()을 주기적으로 호출해야
 * GPIO 핀이 토글됩니다. (실제 커널에서는 timer callback)
 *
 * ============================================================
 * TDD 스킬: 상태 전이 테스트
 * ============================================================
 * 상태 머신 테스트는 세 가지를 검증합니다:
 *   1. 상태 전이 후 현재 상태 (led_fsm_get() 반환값)
 *   2. 상태 전이 시 하드웨어 동작 (gpio_write 호출 확인)
 *   3. 시간에 따른 동작 (tick() 반복 호출 후 GPIO 상태)
 */

#ifndef LED_FSM_H
#define LED_FSM_H

#include "gpio_driver.h"

/* LED 상태 */
typedef enum {
    LED_OFF   = 0,
    LED_ON    = 1,
    LED_BLINK = 2,
} led_state_t;

/*
 * led_fsm_init — 상태 머신 초기화
 *
 * @param ops  GPIO 연산 구조체 (테스트에서는 fake ops 전달)
 * @param pin  제어할 GPIO 핀 번호
 *
 * 초기 상태: LED_OFF, GPIO 핀 LOW 출력
 */
void led_fsm_init(gpio_ops_t *ops, int pin);

/*
 * led_fsm_set — LED 상태 변경
 *
 * 상태 전이 시 즉시 GPIO를 구동합니다:
 *   LED_OFF   → gpio_write(pin, LOW)
 *   LED_ON    → gpio_write(pin, HIGH)
 *   LED_BLINK → gpio_write(pin, LOW)  // tick()이 토글
 */
void led_fsm_set(led_state_t state);

/*
 * led_fsm_get — 현재 상태 반환
 */
led_state_t led_fsm_get(void);

/*
 * led_fsm_tick — BLINK 상태에서 GPIO 토글
 *
 * LED_BLINK 상태일 때만 동작합니다.
 * 실제 드라이버에서는 커널 타이머 콜백으로 주기적으로 호출됩니다.
 * 테스트에서는 수동으로 호출하여 동작을 검증합니다.
 */
void led_fsm_tick(void);

/*
 * led_fsm_reset — 내부 상태 초기화 (테스트 격리용)
 *
 * 각 테스트가 깨끗한 상태에서 시작할 수 있도록
 * 전역 상태를 초기화합니다. (FIRST의 'I': Independent)
 */
void led_fsm_reset(void);

#endif /* LED_FSM_H */
