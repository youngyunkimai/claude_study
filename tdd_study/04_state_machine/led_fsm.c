/*
 * led_fsm.c — Module 4 구현
 *
 * ============================================================
 * TDD 관점에서 보는 전역 상태
 * ============================================================
 * 이 모듈은 전역 변수(g_ops, g_pin, g_state)를 사용합니다.
 * 전역 상태는 테스트 간 오염(test pollution)을 일으킬 수 있습니다.
 *
 * 해결책: led_fsm_reset()으로 각 테스트 전/후에 초기화.
 * test_led_fsm.c에서 모든 테스트는 setup()을 먼저 호출합니다.
 *
 * FIRST의 'I' (Independent): 테스트는 순서에 무관해야 합니다.
 * 전역 상태가 있으면 테스트 순서에 따라 결과가 달라집니다.
 * led_fsm_reset()이 이 문제를 해결합니다.
 *
 * ============================================================
 * 스텁으로 시작하는 방법 (실습)
 * ============================================================
 *
 * STEP 1: 모든 함수를 빈 스텁으로
 *   void led_fsm_init(gpio_ops_t *ops, int pin) { (void)ops; (void)pin; }
 *   void led_fsm_set(led_state_t state) { (void)state; }
 *   led_state_t led_fsm_get(void) { return LED_OFF; }
 *   void led_fsm_tick(void) { }
 *   void led_fsm_reset(void) { }
 *
 * STEP 2: make test → RED 확인
 *
 * STEP 3: 테스트 하나씩 통과시키며 구현 추가
 */

#include "led_fsm.h"
#include <stddef.h>

/* 모듈 전역 상태 */
static gpio_ops_t  *g_ops         = NULL;
static int          g_pin         = 0;
static led_state_t  g_state       = LED_OFF;
static int          g_blink_level = GPIO_LOW;

void led_fsm_init(gpio_ops_t *ops, int pin)
{
    g_ops         = ops;
    g_pin         = pin;
    g_state       = LED_OFF;
    g_blink_level = GPIO_LOW;

    /* 초기화 시 LED를 OFF 상태로 */
    if (g_ops && g_ops->gpio_write)
        g_ops->gpio_write(g_pin, GPIO_LOW);
}

void led_fsm_set(led_state_t state)
{
    g_state = state;

    if (!g_ops || !g_ops->gpio_write)
        return;

    switch (state) {
    case LED_OFF:
        g_ops->gpio_write(g_pin, GPIO_LOW);
        break;
    case LED_ON:
        g_ops->gpio_write(g_pin, GPIO_HIGH);
        break;
    case LED_BLINK:
        /* BLINK 시작: LOW에서 시작, tick()이 토글 */
        g_blink_level = GPIO_LOW;
        g_ops->gpio_write(g_pin, g_blink_level);
        break;
    }
}

led_state_t led_fsm_get(void)
{
    return g_state;
}

void led_fsm_tick(void)
{
    /* BLINK 상태가 아니면 아무것도 하지 않음 */
    if (g_state != LED_BLINK || !g_ops || !g_ops->gpio_write)
        return;

    /* 현재 레벨을 토글 */
    g_blink_level = (g_blink_level == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
    g_ops->gpio_write(g_pin, g_blink_level);
}

void led_fsm_reset(void)
{
    g_ops         = NULL;
    g_pin         = 0;
    g_state       = LED_OFF;
    g_blink_level = GPIO_LOW;
}
