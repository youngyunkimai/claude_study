/*
 * gpio_driver.h — Module 4: 하드웨어 추상화 인터페이스
 *
 * ============================================================
 * 이 모듈에서 배우는 것
 * ============================================================
 *   - 의존성 주입 (Dependency Injection) in C
 *   - 테스트 대역 (Test Double / Fake)
 *   - 하드웨어 없이 드라이버 로직 테스트
 *
 * ============================================================
 * 커널 연관성
 * ============================================================
 * Linux 커널은 ops 구조체로 동일한 패턴을 사용합니다:
 *
 *   struct file_operations {
 *       ssize_t (*read)(struct file *, char __user *, size_t, loff_t *);
 *       ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *);
 *       ...
 *   };
 *
 *   struct i2c_algorithm {
 *       int (*master_xfer)(struct i2c_adapter *, struct i2c_msg *, int);
 *       ...
 *   };
 *
 * 프로덕션 코드는 실제 함수를 넣고,
 * 테스트에서는 fake 함수를 넣습니다.
 * 이것이 C에서 의존성 주입을 구현하는 방법입니다.
 *
 * ============================================================
 * 의존성 주입이 왜 중요한가?
 * ============================================================
 * 하드웨어 없이 테스트 불가능한 코드:
 *
 *   void led_on(void) {
 *       *((volatile uint32_t *)0x40020010) = 1;  // GPIO 레지스터 직접 접근
 *   }
 *
 * 의존성 주입으로 테스트 가능한 코드:
 *
 *   void led_on(gpio_ops_t *ops, int pin) {
 *       ops->gpio_write(pin, GPIO_HIGH);  // 테스트에서는 fake_gpio_write
 *   }
 *
 * 테스트에서 fake_gpio_write는 실제 하드웨어 대신
 * 값을 메모리에 기록합니다. 테스트가 그 값을 검증합니다.
 */

#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

/*
 * gpio_ops_t — GPIO 하드웨어 추상화 (ops 구조체)
 *
 * 이 구조체의 함수 포인터들이 "의존성"입니다.
 * 프로덕션: 실제 GPIO 레지스터를 건드리는 함수
 * 테스트:   fake 함수 (호출 횟수, 값 기록)
 */
typedef struct {
    /* GPIO 핀에 값 출력 (0 또는 1) */
    void (*gpio_write)(int pin, int value);

    /* GPIO 핀 값 읽기 */
    int (*gpio_read)(int pin);

    /* 밀리초 단위 지연 (blink 타이밍 등) */
    void (*delay_ms)(int ms);
} gpio_ops_t;

/* GPIO 값 상수 */
#define GPIO_LOW  0
#define GPIO_HIGH 1

#endif /* GPIO_DRIVER_H */
