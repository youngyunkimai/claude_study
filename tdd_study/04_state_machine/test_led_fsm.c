/*
 * test_led_fsm.c — Module 4: 의존성 주입 & 테스트 대역
 *
 * ============================================================
 * 핵심 패턴: Fake GPIO (테스트 대역)
 * ============================================================
 *
 * 실제 GPIO 하드웨어가 없어도 드라이버 로직을 테스트할 수 있습니다.
 * 비결은 "fake" 함수입니다:
 *
 *   실제 GPIO:
 *     void real_gpio_write(int pin, int value) {
 *         volatile uint32_t *reg = (uint32_t *)GPIO_BASE;
 *         *reg = value;  ← 실제 하드웨어 레지스터
 *     }
 *
 *   Fake GPIO (테스트용):
 *     void fake_gpio_write(int pin, int value) {
 *         fake_pin_state = value;    ← 메모리에 저장
 *         fake_write_count++;        ← 호출 횟수 추적
 *     }
 *
 * 테스트는 fake_pin_state와 fake_write_count로 동작을 검증합니다.
 *
 * ============================================================
 * 이 파일에서 배우는 TDD 스킬
 * ============================================================
 *   1. 테스트 대역(Test Double) 작성법
 *   2. 호출 횟수(call count) 검증
 *   3. 상태 전이 시퀀스 테스트
 *   4. 시간에 따른 동작 (tick 반복) 테스트
 *   5. 테스트 셋업/티어다운 패턴
 */

#include "../framework/test_framework.h"
#include "led_fsm.h"

/* ===========================================================
 * FAKE GPIO — 테스트 대역 (Test Double)
 *
 * 이 변수들이 "관찰 도구"입니다.
 * 테스트는 이 값들을 보고 "드라이버가 올바르게 GPIO를 제어했는가"를 판단합니다.
 * =========================================================== */

static int fake_pin_state   = GPIO_LOW;   /* 현재 핀 출력값 */
static int fake_write_count = 0;           /* gpio_write 호출 횟수 */

static void fake_gpio_write(int pin, int value)
{
    (void)pin;  /* 이 테스트에서는 핀 번호 무시 */
    fake_pin_state = value;
    fake_write_count++;
}

static int fake_gpio_read(int pin)
{
    (void)pin;
    return fake_pin_state;
}

static void fake_delay_ms(int ms)
{
    (void)ms;  /* 테스트에서는 실제로 기다리지 않음 (Fast: FIRST의 F) */
}

/* fake ops 구조체 — 테스트에서 led_fsm_init에 전달 */
static gpio_ops_t fake_ops = {
    .gpio_write = fake_gpio_write,
    .gpio_read  = fake_gpio_read,
    .delay_ms   = fake_delay_ms,
};

/* ===========================================================
 * 셋업 헬퍼
 *
 * 각 테스트 전에 호출하여 깨끗한 상태를 보장합니다.
 * (FIRST의 'I': Independent — 테스트 순서 무관)
 * =========================================================== */
static void setup(void)
{
    /* FSM 내부 상태 초기화 */
    led_fsm_reset();

    /* Fake GPIO 관찰 변수 초기화 */
    fake_pin_state   = GPIO_LOW;
    fake_write_count = 0;
}

/* ===========================================================
 * 테스트 케이스
 * =========================================================== */

/* ----------------------------------------------------------
 * 초기 상태 테스트
 * ---------------------------------------------------------- */

void test_fsm_initial_state_is_off(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);

    TEST_ASSERT_EQUAL_INT(LED_OFF, led_fsm_get());
}

void test_fsm_init_drives_gpio_low(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);

    /* 초기화 시 LED_OFF → GPIO_LOW 출력 */
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);
    TEST_ASSERT_EQUAL_INT(1, fake_write_count);
}

/* ----------------------------------------------------------
 * LED_ON 상태 전이
 * ---------------------------------------------------------- */

void test_fsm_set_on_changes_state_to_on(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);

    led_fsm_set(LED_ON);

    TEST_ASSERT_EQUAL_INT(LED_ON, led_fsm_get());
}

void test_fsm_set_on_drives_gpio_high(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);

    fake_write_count = 0;  /* init 호출 횟수 리셋 */

    led_fsm_set(LED_ON);

    /* LED_ON → GPIO_HIGH 출력 */
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);
    /* gpio_write가 정확히 1회 호출되어야 함 */
    TEST_ASSERT_EQUAL_INT(1, fake_write_count);
}

/* ----------------------------------------------------------
 * LED_OFF 상태 전이
 * ---------------------------------------------------------- */

void test_fsm_set_off_from_on_drives_gpio_low(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_ON);

    fake_write_count = 0;
    led_fsm_set(LED_OFF);

    TEST_ASSERT_EQUAL_INT(LED_OFF, led_fsm_get());
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);
    TEST_ASSERT_EQUAL_INT(1, fake_write_count);
}

/* ----------------------------------------------------------
 * LED_BLINK 상태 전이 + tick() 동작
 *
 * tick() 동작 검증이 이 모듈의 핵심 테스트입니다.
 * 하드웨어 없이 "시간에 따른 동작"을 테스트합니다.
 * ---------------------------------------------------------- */

void test_fsm_set_blink_changes_state(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_BLINK);

    TEST_ASSERT_EQUAL_INT(LED_BLINK, led_fsm_get());
}

void test_fsm_set_blink_starts_with_gpio_low(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_BLINK);

    /* BLINK 시작 시 LOW에서 시작 */
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);
}

void test_fsm_tick_in_blink_toggles_gpio(void)
{
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_BLINK);  /* LOW에서 시작 */

    /* 첫 번째 tick: LOW → HIGH */
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);

    /* 두 번째 tick: HIGH → LOW */
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);

    /* 세 번째 tick: LOW → HIGH */
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);
}

void test_fsm_tick_when_not_blink_does_nothing(void)
{
    /*
     * LED_ON 상태에서 tick()을 호출해도 GPIO 상태가 변하지 않아야 함.
     * tick()은 LED_BLINK 상태에서만 동작합니다.
     */
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_ON);

    fake_write_count = 0;
    fake_pin_state   = GPIO_HIGH;  /* LED_ON 상태 반영 */

    led_fsm_tick();

    /* gpio_write가 호출되지 않아야 함 */
    TEST_ASSERT_EQUAL_INT(0, fake_write_count);
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);
}

void test_fsm_blink_to_off_stops_toggling(void)
{
    /*
     * BLINK → OFF 전이 후 tick()을 호출해도 GPIO가 토글되지 않아야 함.
     * 상태 전이가 tick() 동작에 영향을 미치는지 검증합니다.
     */
    setup();
    led_fsm_init(&fake_ops, 1);
    led_fsm_set(LED_BLINK);

    /* BLINK 중 tick 한 번 */
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(GPIO_HIGH, fake_pin_state);

    /* OFF로 전환 */
    led_fsm_set(LED_OFF);
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);

    /* OFF 상태에서 tick → 변화 없어야 함 */
    fake_write_count = 0;
    led_fsm_tick();
    TEST_ASSERT_EQUAL_INT(0, fake_write_count);
    TEST_ASSERT_EQUAL_INT(GPIO_LOW, fake_pin_state);
}

/* ----------------------------------------------------------
 * main
 * ---------------------------------------------------------- */
int main(void)
{
    printf("Module 4: LED State Machine — 의존성 주입 & 테스트 대역\n");
    printf("------------------------------------------------------\n");
    printf("핵심 패턴: 실제 GPIO 없이 fake_gpio로 드라이버 로직 테스트\n");
    printf("------------------------------------------------------\n");

    /* 초기 상태 */
    RUN_TEST(test_fsm_initial_state_is_off);
    RUN_TEST(test_fsm_init_drives_gpio_low);

    /* LED_ON 전이 */
    RUN_TEST(test_fsm_set_on_changes_state_to_on);
    RUN_TEST(test_fsm_set_on_drives_gpio_high);

    /* LED_OFF 전이 */
    RUN_TEST(test_fsm_set_off_from_on_drives_gpio_low);

    /* LED_BLINK + tick() */
    RUN_TEST(test_fsm_set_blink_changes_state);
    RUN_TEST(test_fsm_set_blink_starts_with_gpio_low);
    RUN_TEST(test_fsm_tick_in_blink_toggles_gpio);
    RUN_TEST(test_fsm_tick_when_not_blink_does_nothing);
    RUN_TEST(test_fsm_blink_to_off_stops_toggling);

    TEST_SUMMARY();
    return TEST_EXIT_CODE;
}
