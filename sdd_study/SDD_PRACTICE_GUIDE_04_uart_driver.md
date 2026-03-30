# SDD 실습 가이드 — UART Driver (프롬프트 기반)

> AI에게 프롬프트를 입력하면서 SDD 사이클을 직접 체험하는 실습입니다.
> 각 단계의 프롬프트를 그대로 복사해서 Claude에게 입력하세요.

---

## 이 모듈이 앞선 모듈들과 다른 점

| | Stack (01) | UART Driver (04) |
|--|-----------|-----------------|
| 상태 | 단순 (empty/full) | **상태 기계** (init→open→rw→close) |
| 의존성 | 없음 | **하드웨어 레지스터** |
| 에러 유형 | 경계값 | **하드웨어 오류 + 컨텍스트 오류** |
| 명세 대상 | 함수 동작 | 함수 동작 + **하드웨어 부수효과** |
| 핵심 개념 | @pre/@post | + **@state, @modifies 레지스터** |

> UART 드라이버는 실제 하드웨어 없이 명세만으로 설계를 완성할 수 있음을 보여주는 가장 좋은 SDD 예제다.

---

## 실습 구조

```
STEP 1  하드웨어 이해 — UART 레지스터와 동작 원리 파악
   ↓
STEP 2  상태 기계 명세 — 드라이버의 상태 전이 정의
   ↓
STEP 3  인터페이스 설계 — API 함수 목록 결정
   ↓
STEP 4  형식적 명세 — @pre / @post / @state / @modifies 계약 주석
            4-1. uart_init() 명세
            4-2. uart_open() / uart_close() 명세
            4-3. uart_write() / uart_read() 명세
            4-4. 드라이버 불변식 정의
   ↓
STEP 5  계약 코드 — contract.h + uart_driver.h (구현 없음)
            5-1. uart_driver.h (계약 주석 + 함수 선언)
            5-2. Makefile
   ↓
STEP 6  명세 기반 테스트 — 상태 전이 속성 테스트 작성 (RED)
            6-1. 상태 전이 속성 목록 도출
            6-2. test_uart_driver.c 작성
   ↓
STEP 7  HAL 스텁 구현 — 하드웨어를 가짜로 대체
   ↓
STEP 8  드라이버 구현 — REQUIRE/ENSURE 포함한 uart_driver.c (GREEN)
   ↓
STEP 9  계약 위반 실험 — 잘못된 상태에서 함수 호출
```

---

## STEP 1. 하드웨어 이해

### 목표
명세를 쓰려면 무엇을 명세해야 하는지 알아야 합니다.
UART 하드웨어의 기본 동작 원리를 파악합니다.

### 프롬프트

```
임베디드 C 드라이버 개발을 위해 UART 하드웨어의 기본 동작을 설명해줘.

아래 항목만 간결하게 설명해줘 (코드 없음):
1. UART가 하는 일 (한 줄)
2. 드라이버가 제어해야 하는 핵심 레지스터 4가지와 각각의 역할
   (THR, RHR, LSR, LCR 중심으로)
3. 데이터 송신 흐름 (레지스터 관점에서 3단계로)
4. 데이터 수신 흐름 (레지스터 관점에서 3단계로)
5. 드라이버 초기화 시 설정해야 할 것 (baud rate, 데이터 비트 등)

임베디드 C 드라이버 개발자 관점에서 설명해줘.
```

### 확인 포인트

- [ ] THR (Transmit Holding Register): 송신 데이터를 쓰는 레지스터
- [ ] RHR (Receive Holding Register): 수신 데이터를 읽는 레지스터
- [ ] LSR (Line Status Register): 송수신 가능 상태를 알려주는 레지스터
- [ ] LCR (Line Control Register): baud rate, 데이터 비트 등을 설정하는 레지스터
- [ ] 송신 전 `LSR.THRE(Transmit Holding Register Empty)` 확인이 필요함을 이해했다
- [ ] 수신 전 `LSR.DR(Data Ready)` 확인이 필요함을 이해했다

### 왜 이 단계가 중요한가

> 드라이버 명세에서 `@modifies UART_THR`이라고 쓰려면
> THR이 무엇인지 알아야 한다.
> 하드웨어를 이해하지 않으면 의미 있는 명세를 쓸 수 없다.

---

## STEP 2. 상태 기계 명세

### 상태 기계 명세란?

시스템이 **항상 하나의 상태(State)** 안에 있고, **특정 이벤트(함수 호출 등)** 에 의해
다른 상태로 **전이(Transition)** 하는 모델을 형식적으로 정의한 것입니다.

```
상태 기계 = 상태(State) + 전이(Transition) + 이벤트(Event)
```

UART 드라이버는 어느 순간에도 반드시 아래 4가지 상태 중 **하나**입니다:

```
UNINITIALIZED → INITIALIZED → OPEN → (다시 INITIALIZED)
                                                                         ↘
                                                                          ERROR
```

각 상태에서 **허용되는 함수 호출이 다릅니다**:

| 현재 상태 | 호출 가능한 함수 | 호출 불가능한 함수 |
|----------|----------------|-----------------|
| UNINITIALIZED | `uart_init()` | `uart_open()`, `uart_write()`, `uart_read()` |
| INITIALIZED | `uart_open()` | `uart_write()`, `uart_read()` |
| OPEN | `uart_write()`, `uart_read()`, `uart_close()` | `uart_init()`, `uart_open()` |
| ERROR | `uart_close()` | 나머지 전부 |

#### 명세가 없는 경우 vs 있는 경우

```c
// 명세 없음: 암묵적 지식 — 개발자 머릿속에만 존재
uart_open(dev);  // init 안 했는데 → 원인 모를 크래시 발생

// 명세 있음: @state로 계약 명문화 + REQUIRE로 런타임 강제
// @state dev->state == UART_STATE_INITIALIZED
REQUIRE(dev->state == UART_STATE_INITIALIZED);
// → 위반 시: "Precondition failed: dev->state == UART_STATE_INITIALIZED at uart_driver.c:87"
```

#### 상태 기계 명세의 3가지 구성 요소

**1. 상태 목록** — 가능한 모든 상태를 열거
```c
typedef enum {
    UART_STATE_UNINITIALIZED,
    UART_STATE_INITIALIZED,
    UART_STATE_OPEN,
    UART_STATE_ERROR,
} uart_state_t;
```

**2. 전이 조건** — 어떤 함수가 어떤 상태 전이를 일으키는가
```
uart_init()  성공: UNINITIALIZED → INITIALIZED
uart_init()  실패: UNINITIALIZED → ERROR
uart_open()  성공: INITIALIZED   → OPEN
uart_close() 성공: OPEN          → INITIALIZED
```

**3. 상태 사전조건 (@state)** — 각 함수가 어떤 상태에서만 호출 가능한가
```
uart_open()  @state == INITIALIZED   (다른 상태에서 호출 → REQUIRE 위반)
uart_write() @state == OPEN
uart_close() @state == OPEN
```

#### 일반 @pre와 @state의 차이

```
@pre   → 파라미터/값 조건    예: buf != NULL, len > 0
@state → 드라이버 상태 조건  예: dev->state == UART_STATE_OPEN
```

둘 다 사전조건이지만 구분하는 이유는, `@state` 위반은 **호출 순서 버그**임을 명확히 하기 위해서다.

---

### 목표
UART 드라이버가 가질 수 있는 **상태(State)**와
각 상태 간의 **전이(Transition)**를 정의합니다.
이것이 `@state` 사전조건의 기반이 됩니다.

### 프롬프트 2-1: 상태 목록 정의

```
UART 드라이버의 상태 기계를 설계해줘.

조건:
- 드라이버가 가질 수 있는 상태를 enum으로 정의
- 각 상태의 의미를 한 줄로 설명
- 하드웨어 초기화 이전부터 정상 동작, 에러까지 모두 포함

아래 형식으로:
  상태 이름: UART_STATE_XXX
  의미: ...
  이 상태에서 호출 가능한 함수: ...

코드 없이 표 형식으로만 작성.
```

### 확인 포인트

- [ ] `UNINITIALIZED` (초기화 전): `uart_init()`만 호출 가능
- [ ] `INITIALIZED` (초기화 완료): `uart_open()`만 호출 가능
- [ ] `OPEN` (열림): `uart_read()`, `uart_write()`, `uart_close()` 호출 가능
- [ ] `ERROR` (하드웨어 오류): `uart_reset()` 또는 `uart_close()`만 호출 가능
- [ ] 각 상태에서 허용되지 않는 함수 호출이 사전조건 위반임을 이해했다

---

### 프롬프트 2-2: 상태 전이 다이어그램

```
STEP 2-1에서 정의한 상태들로 상태 전이 다이어그램을 ASCII 아트로 그려줘.

포함할 내용:
- 각 상태 노드 (박스)
- 전이 조건 (화살표 위에 함수명과 성공/실패)
- 성공 시 전이와 에러 시 전이를 구분

예시 형식:
  [UNINITIALIZED] --uart_init() 성공--> [INITIALIZED]
                  --uart_init() 실패--> [ERROR]
```

### 확인 포인트

- [ ] 모든 상태가 노드로 표현되어 있다
- [ ] 각 함수 호출이 어떤 상태 전이를 일으키는지 명확하다
- [ ] 에러 상태로의 전이 경로가 있다
- [ ] `uart_close()`가 `OPEN → INITIALIZED`로 복귀하는 경로가 있다

### 왜 상태 기계 명세가 중요한가

```
잘못된 상태에서 함수 호출 → 가장 찾기 어려운 버그 유형

예: OPEN 상태에서 uart_init()을 다시 호출하면?
  - 명세 없음: "그냥 해봐야 알지"
  - SDD:       @pre dev->state == UART_STATE_UNINITIALIZED
               → 위반 즉시 REQUIRE가 abort() 호출
               → 어디서 잘못 호출했는지 즉시 파악
```

---

## STEP 3. 인터페이스 설계

### 목표
상태 기계를 기반으로 드라이버 API를 결정합니다.

### 프롬프트

```
STEP 1, 2에서 파악한 UART 하드웨어와 상태 기계를 바탕으로
uart_driver C 라이브러리의 인터페이스를 설계해줘.

조건:
- 함수: uart_init, uart_open, uart_close, uart_write, uart_read, uart_get_state
- 에러는 반환값으로 표현 (0=성공, 음수=에러)
- 에러 코드를 enum으로 정의 (UART_OK, UART_ERR_INVAL, UART_ERR_BUSY,
  UART_ERR_TIMEOUT, UART_ERR_HW, UART_ERR_STATE)
- uart_config_t 구조체 (baud_rate, data_bits, stop_bits, parity 필드)
- uart_dev_t 구조체 (state, base_addr, config 필드)

아래 형식으로 작성 (코드 없음):
  | 함수 | 파라미터 | 반환값 | 유효한 호출 상태 |
```

### 확인 포인트

- [ ] 6개 함수의 시그니처가 결정되었다
- [ ] 에러 코드 6개가 정의되었다
- [ ] `uart_config_t`와 `uart_dev_t` 구조체 필드가 결정되었다
- [ ] "유효한 호출 상태" 열이 있다 → 이것이 `@state` 사전조건이 된다

---

## STEP 4. 형식적 명세 작성

### 형식적 명세란?

"형식적"은 **기계가 읽고 판단할 수 있는 엄밀한 표현**이라는 뜻입니다.

| | 비형식적 (Informal) | 형식적 (Formal) |
|--|-------------------|----------------|
| 표현 방식 | 자연어 (한국어, 영어) | 논리식, 수식, 코드 |
| 모호성 | 있음 | 없음 |
| 기계 검증 | 불가 | 가능 |
| 예 | "버퍼가 비어있으면 안 됨" | `buf != NULL && len > 0` |

#### 비형식적 명세 vs 형식적 명세

`uart_write()`를 예로 들면:

```c
// 비형식적: "열려있어야 한다"가 무슨 뜻인지 해석이 다를 수 있음
// 데이터를 UART로 전송한다.
// 버퍼가 있어야 하고, 드라이버가 열려있어야 한다.
int uart_write(uart_dev_t *dev, const uint8_t *buf, size_t len);

// 형식적: 논리식으로 표현 — 모호성 없음, 코드로 1:1 번역 가능
// @state  dev->state == UART_STATE_OPEN
// @pre    dev != NULL
// @pre    buf != NULL
// @pre    len > 0
// @post   result >= 0  ==>  result <= len
// @post   result < 0   ==>  dev->state == UART_STATE_ERROR
// @modifies UART_THR_REG, dev->state
int uart_write(uart_dev_t *dev, const uint8_t *buf, size_t len);
```

#### 형식적 명세 → 코드 직접 번역

명세가 논리식이면 해석 없이 그대로 코드로 옮겨집니다:

```
명세 (형식적)                          코드 (REQUIRE/ENSURE)
──────────────────────────────────     ──────────────────────────────────
@state dev->state == UART_STATE_OPEN   REQUIRE(dev->state == UART_STATE_OPEN)
@pre   buf != NULL                     REQUIRE(buf != NULL)
@pre   len > 0                         REQUIRE(len > 0)
@post  result >= 0 ==> result <= len   ENSURE(result < 0 || result <= len)
```

#### `==>` (함의)가 핵심

`@post`에서 자주 쓰이는 `==>` 는 "A이면 B이다"라는 조건부 논리식입니다:

```
result == 0  ==>  dev->state == UART_STATE_INITIALIZED
→ "성공했다면, 상태는 반드시 INITIALIZED다"
→ 실패했을 때는 상태가 무엇이든 이 명세를 위반하지 않는다
```

---

### 목표
각 함수의 계약을 `@pre`, `@post`, `@state`, `@modifies`로 완전히 정의합니다.

### 새로운 명세 키워드: `@state`와 `@modifies`

```
@state  (상태 사전조건) — 이 함수를 호출할 수 있는 드라이버 상태
  예:  @state dev->state == UART_STATE_INITIALIZED

@modifies (부수효과 명세) — 이 함수가 변경하는 것들
  예:  @modifies dev->state
       @modifies UART_LCR_REG  ← 하드웨어 레지스터까지 명세
```

---

### 프롬프트 4-1: uart_init() 명세

```
uart_init() 함수의 형식적 명세를 작성해줘.

함수 시그니처:
  int uart_init(uart_dev_t *dev, uintptr_t base_addr, const uart_config_t *cfg);

아래 키워드로 명세:
  @state    (호출 가능한 드라이버 상태)
  @pre      (파라미터 사전조건)
  @post     (성공/실패 각각의 사후조건)
  @modifies (변경되는 소프트웨어 상태 + 하드웨어 레지스터)
  @return   (각 반환값의 의미)

추가 조건:
- base_addr 유효성 조건 포함
- baud_rate 유효 범위 조건 포함 (예: 9600~115200)
- 하드웨어 레지스터 초기화 순서를 @modifies에 반영
- 코드 없이 명세만 작성
```

### 확인 포인트

- [ ] `@state dev->state == UART_STATE_UNINITIALIZED` 가 있다
- [ ] `@pre cfg->baud_rate` 범위 조건이 있다
- [ ] `@post (result==0) ==> dev->state == UART_STATE_INITIALIZED` 가 있다
- [ ] `@modifies` 에 LCR, DLL, DLH 등 초기화 레지스터가 포함되어 있다

---

### 프롬프트 4-2: uart_open() / uart_close() 명세

```
uart_open()과 uart_close() 함수의 형식적 명세를 작성해줘.

함수 시그니처:
  int uart_open(uart_dev_t *dev);
  int uart_close(uart_dev_t *dev);

각 함수에 대해:
  @state / @pre / @post / @modifies / @return

추가 조건:
- uart_open: 송수신 인터럽트 활성화를 @modifies에 포함
- uart_close: 상태가 INITIALIZED로 복귀하는 사후조건 포함
- uart_close: OPEN 상태가 아닐 때의 동작도 명세에 포함
- 코드 없이 명세만 작성
```

### 확인 포인트

- [ ] `uart_open`의 `@state == UART_STATE_INITIALIZED` 가 있다
- [ ] `uart_open` 성공 후 `@post dev->state == UART_STATE_OPEN` 이 있다
- [ ] `uart_close`의 `@post dev->state == UART_STATE_INITIALIZED` 가 있다

---

### 프롬프트 4-3: uart_write() / uart_read() 명세

```
uart_write()와 uart_read() 함수의 형식적 명세를 작성해줘.

함수 시그니처:
  int uart_write(uart_dev_t *dev, const uint8_t *buf, size_t len);
  int uart_read(uart_dev_t *dev, uint8_t *buf, size_t len);

각 함수에 대해:
  @state / @pre / @post / @modifies / @return

추가 조건:
- uart_write:
    * LSR.THRE 확인 후 THR에 쓰는 과정을 @modifies에 포함
    * 반환값이 실제 전송된 바이트 수임을 명세
    * 타임아웃 조건과 반환값 포함
- uart_read:
    * LSR.DR 확인 후 RHR에서 읽는 과정을 @modifies에 포함
    * 수신 데이터가 없을 때의 반환값(0 또는 TIMEOUT) 명세
    * buf가 NULL이 아님 + len > 0 사전조건 포함
- 두 함수 모두 @state == UART_STATE_OPEN 포함
- 코드 없이 명세만 작성
```

### 확인 포인트

- [ ] `uart_write`의 `@modifies UART_THR_REG` 가 있다
- [ ] `uart_read`의 `@modifies UART_RHR_REG` 가 있다 (읽기도 부수효과)
- [ ] 타임아웃 시 반환값(`UART_ERR_TIMEOUT`)이 @return에 있다
- [ ] `@post result >= 0 ==> result <= len` 이 있다

---

### 프롬프트 4-4: 드라이버 불변식 정의

```
uart_dev_t의 불변식(Invariant)을 정의해줘.

uart_dev_t 구조체:
  uart_state_t  state;
  uintptr_t     base_addr;
  uart_config_t config;

아래 조건으로 불변식을 5개 이상 작성:
  @invariant [조건] — [이유]

포함할 내용:
- state 값의 유효 범위
- base_addr 정렬 조건 (UART 레지스터는 보통 4바이트 정렬)
- baud_rate 유효 범위
- state와 하드웨어 레지스터 상태의 일관성
  (예: state==OPEN 이면 UART_IER(인터럽트 활성 레지스터)가 설정되어 있음)

코드 없이 조건식과 이유만 작성.
```

### 확인 포인트

- [ ] `state ∈ {UNINITIALIZED, INITIALIZED, OPEN, ERROR}` 범위 불변식이 있다
- [ ] `base_addr % 4 == 0` 정렬 불변식이 있다
- [ ] `state == OPEN` 일 때의 하드웨어 상태 불변식이 있다

---

## STEP 5. 계약 코드 작성

### 목표
STEP 4의 명세를 헤더 파일 주석으로 옮깁니다.
구현(`uart_driver.c`)은 아직 없습니다.

### 프롬프트 5-1: uart_driver.h 작성

```
uart_driver.h 헤더 파일을 작성해줘.

조건:
- STEP 2에서 정의한 uart_state_t enum 포함
- STEP 3에서 설계한 uart_config_t, uart_dev_t 구조체 포함
- STEP 3에서 정의한 에러 코드 enum 포함
- STEP 4에서 작성한 @state/@pre/@post/@modifies/@return 주석을
  모든 함수에 포함
- 레지스터 주소 오프셋 매크로 포함
  (UART_THR_OFFSET, UART_RHR_OFFSET, UART_LSR_OFFSET, UART_LCR_OFFSET 등)
- 함수 선언만, 구현 없음
- #include "contract.h" 포함
- include guard 포함
```

### 확인 포인트

- [ ] `uart_state_t` enum에 4개 상태가 있다
- [ ] 에러 코드 enum에 6개 값이 있다
- [ ] 모든 함수에 `@state`, `@pre`, `@post`, `@modifies` 주석이 있다
- [ ] 레지스터 오프셋 매크로가 정의되어 있다
- [ ] 구현 코드가 없다

---

### 프롬프트 5-2: Makefile 작성

```
sdd_study/04_uart_driver/ 프로젝트를 빌드하는 Makefile을 작성해줘.

조건:
- 컴파일러: gcc
- 컴파일 옵션: -Wall -Wextra -std=c11 -g
- 빌드 대상: test_runner
- 소스: test_uart_driver.c uart_driver.c hal_stub.c
- 헤더 경로: . 및 ../contract 포함
- make test: 빌드 후 ./test_runner 자동 실행
- make clean: *.o와 test_runner 삭제
- .PHONY 포함
```

### 이 시점의 파일 구조

```
sdd_study/04_uart_driver/
├── contract.h       ← 01_stack에서 재사용 (또는 복사)
├── uart_driver.h    ← 계약 주석 + 선언 (구현 없음)
└── Makefile
```

> 아직 구현 파일이 하나도 없다.
> 명세(=헤더)만으로 다른 팀원이 테스트를 먼저 작성할 수 있다 — 이것이 SDD의 협업 효과다.

---

## STEP 6. 명세 기반 테스트 작성

### 목표
STEP 4의 명세에서 **상태 전이 속성**을 추출하여 테스트로 변환합니다.
실제 UART 하드웨어 없이 테스트를 먼저 완성합니다.

### 프롬프트 6-1: 상태 전이 속성 목록

```
STEP 4의 uart 드라이버 명세에서 테스트할 수 있는 속성(Property)을 뽑아줘.

아래 분류로 각 2개 이상:
  1. 상태 전이 속성 (함수 호출 후 state 변화)
  2. 정상 동작 속성 (write/read 데이터 일관성)
  3. 에러 처리 속성 (잘못된 파라미터, 타임아웃)
  4. 불변식 속성 (모든 공개 함수 호출 후 불변식 유지)
  5. 순서 의존성 속성 (init 없이 open 불가 등)

각 속성의 형식:
  속성 이름: [이름]
  검증 방법: [어떻게 테스트하는가]
  관련 명세: [@pre/@post 중 어떤 것에서 왔는가]
```

### 확인 포인트

- [ ] 10개 이상의 속성이 도출되었다
- [ ] "init 없이 open을 호출하면 에러" 같은 순서 의존성 속성이 있다
- [ ] "write 후 read하면 같은 데이터" 같은 데이터 일관성 속성이 있다
- [ ] 각 속성이 STEP 4의 특정 @pre/@post와 연결되어 있다

---

### 프롬프트 6-2: test_uart_driver.c 작성 (RED)

```
위 속성 목록을 C 테스트 코드로 작성해줘.

조건:
- 파일명: test_uart_driver.c
- 테스트 프레임워크: 아래 매크로 사용 (tdd_study/framework/test_framework.h 사용)
    TEST_ASSERT_EQUAL_INT(expected, actual)
    RUN_TEST(test_func_name)
    TEST_SUMMARY()
    TEST_EXIT_CODE
- 헤더: #include "../../tdd_study/framework/test_framework.h"
         #include "uart_driver.h"
- uart_driver.c와 hal_stub.c는 아직 없음 (RED 상태)
- 테스트 이름: test_[속성이름] 형식
- 하드웨어 접근은 HAL 함수(hal_read_reg, hal_write_reg)를 통해 추상화
  → HAL 함수는 아직 없고 나중에 스텁으로 구현됨
- main()에서 RUN_TEST로 모든 테스트 실행
```

### RED 확인

```bash
cd sdd_study/04_uart_driver
make test
```

### 기대 결과 (RED)

```
uart_driver.c: No such file or directory
hal_stub.c: No such file or directory
```

> **RED입니다.**
> 하드웨어도 없고, 구현도 없고, 스텁도 없습니다.
> 그러나 명세와 테스트 의도는 완성되어 있습니다.

---

## STEP 7. HAL 스텁 구현

### 목표
실제 UART 하드웨어를 **메모리 배열로 흉내내는 가짜 HAL**을 만듭니다.
이 단계가 UART 드라이버 SDD의 핵심 설계 패턴입니다.

### HAL이란?

```
HAL (Hardware Abstraction Layer, 하드웨어 추상화 계층):

실제 코드:
  uart_driver.c → UART_THR 레지스터 주소에 직접 접근
  → 하드웨어 없이 테스트 불가

HAL 패턴:
  uart_driver.c → hal_write_reg(dev, UART_THR_OFFSET, data)
                           ↓
  [실제]  hal.c     → *(volatile uint32_t *)(dev->base_addr + offset) = data
  [테스트] hal_stub.c → fake_regs[offset] = data  ← 메모리 배열에 저장

→ 드라이버 코드를 변경하지 않고 하드웨어 유무에 따라 HAL만 교체
```

### 프롬프트 7-1: hal.h 인터페이스 설계

```
UART 드라이버에서 하드웨어를 추상화하는 hal.h를 작성해줘.

조건:
- 함수: hal_write_reg(dev, offset, value), hal_read_reg(dev, offset)
- 실제 구현은 없음 (선언만)
- hal_stub.c에서 구현될 예정임을 주석으로 표시
- HAL을 왜 쓰는지 파일 상단 주석으로 설명
- include guard 포함
```

---

### 프롬프트 7-2: hal_stub.c 작성

```
테스트용 HAL 스텁인 hal_stub.c를 작성해줘.

조건:
- 레지스터를 uint32_t fake_regs[16] 배열로 시뮬레이션
- hal_write_reg(): fake_regs[offset/4] = value
- hal_read_reg():  return fake_regs[offset/4]
- hal_stub_reset(): fake_regs 전체를 0으로 초기화 (테스트 시작 시 호출)
- hal_stub_set_lsr(): LSR 레지스터를 원하는 값으로 설정하는 테스트 헬퍼
  → 예: hal_stub_set_lsr(LSR_THRE | LSR_DR) → 송수신 모두 준비됨으로 설정
- 각 함수에 어떤 역할인지 주석 포함
```

### 확인 포인트

- [ ] `hal_stub_reset()`으로 각 테스트를 독립적으로 초기화할 수 있다
- [ ] `hal_stub_set_lsr()`로 LSR 상태를 제어해서 "데이터 수신 대기" 상황을 만들 수 있다
- [ ] `hal_read_reg()`가 `fake_regs`에서 읽어온다
- [ ] 드라이버 코드(`uart_driver.c`)는 HAL 함수만 호출하고 직접 레지스터에 접근하지 않는다

---

## STEP 8. 드라이버 구현 (GREEN)

### 목표
명세의 계약(`REQUIRE/ENSURE`)을 포함해서 드라이버를 구현합니다.
`hal_write_reg` / `hal_read_reg`만 사용하고 직접 레지스터에 접근하지 않습니다.

### 프롬프트 8-1: uart_driver.c 작성

```
uart_driver.c를 작성해줘.

조건:
- uart_driver.h의 모든 @pre를 REQUIRE()로 구현
- uart_driver.h의 모든 @post를 ENSURE()로 구현
- 불변식을 check_invariants(dev) 내부 함수로 분리하여 각 공개 함수 끝에서 호출
- 하드웨어 접근은 반드시 hal_write_reg() / hal_read_reg()만 사용
- 각 REQUIRE/ENSURE에 어떤 @pre/@post에 해당하는지 주석으로 표시
- TDD Law 3 적용: 테스트를 통과할 최소 구현만 작성
```

### GREEN 확인

```bash
make test
```

### 기대 결과 (GREEN)

```
[RUN ] test_init_transitions_state_to_initialized      PASS
[RUN ] test_open_requires_initialized_state            PASS
[RUN ] test_open_transitions_state_to_open             PASS
[RUN ] test_write_sends_data_to_thr                    PASS
[RUN ] test_read_gets_data_from_rhr                    PASS
[RUN ] test_close_transitions_state_to_initialized     PASS
[RUN ] test_invariant_base_addr_alignment              PASS
[RUN ] test_double_open_returns_error                  PASS
[RUN ] test_write_without_open_returns_error           PASS
...
======================================================
  ALL PASSED  |  Total: N  |  Passed: N  |  Failed:  0
======================================================
```

---

## STEP 9. 계약 위반 실험

### 목표
잘못된 상태에서 함수를 호출해서 `REQUIRE`가 즉시 문제를 잡아내는 것을 확인합니다.

### 프롬프트 9-1: 상태 위반 실험 코드

```
아래 세 가지 계약 위반 상황을 실험하는 C 코드를 작성해줘.
파일명: test_contract_violation.c

실험 1: init 없이 open 호출
  - uart_dev_t를 선언만 하고 (init 없이) uart_open() 호출
  - 기대: @state 위반으로 REQUIRE 메시지 출력 후 abort

실험 2: close 없이 init 재호출
  - uart_init() 후 uart_open() 후
  - uart_close() 없이 uart_init()을 다시 호출
  - 기대: @state 위반으로 REQUIRE 메시지 출력 후 abort

실험 3: NULL 버퍼로 write
  - uart_init() → uart_open() 정상 진행 후
  - uart_write(dev, NULL, 10) 호출
  - 기대: @pre buf != NULL 위반으로 REQUIRE 메시지 후 abort

조건:
- 각 실험 전에 printf로 실험 번호와 내용 출력
- Makefile에 make contract_test 타겟 추가
```

### 실험 실행

```bash
make contract_test
```

### 기대 결과

```
=== 실험 1: init 없이 open 호출 ===
Precondition failed: dev->state == UART_STATE_INITIALIZED
  at uart_driver.c:87
Aborted (core dumped)
```

### 확인 포인트

- [ ] 위반 메시지에 **상태 조건식 문자열**이 그대로 출력된다
- [ ] **파일명과 라인 번호**가 출력된다 → 어디서 잘못 호출했는지 즉시 파악
- [ ] `make test`의 정상 테스트는 여전히 ALL PASSED다

---

## TDD vs SDD: 이 모듈에서 배운 차이

```
TDD로 UART 드라이버를 만들었다면:
  - "uart_write(dev, buf, 5) 후 hal_read_reg로 THR 값을 확인" 같은 구체적 예제 테스트
  - init 없이 open을 호출하는 케이스를 테스트에서 빠뜨리기 쉬움

SDD로 만들었기 때문에:
  - @state 명세가 있어서 "init 없는 open"이 명세 위반임이 명확
  - REQUIRE가 코드 안에 있어서 위반 즉시 abort → 버그 위치 즉시 파악
  - HAL 분리로 하드웨어 없이 모든 명세를 테스트 가능

결론:
  TDD는 "이 입력에 이 출력이 나오는가"를 검증한다.
  SDD는 "이 함수가 계약을 지키고 있는가"를 검증한다.
```

---

## 최종 파일 구조

```
sdd_study/04_uart_driver/
├── contract.h                  ← REQUIRE/ENSURE/INVARIANT 매크로
├── hal.h                       ← HAL 인터페이스 (선언)
├── hal_stub.c                  ← 테스트용 가짜 HAL (fake_regs 배열)
├── uart_driver.h               ← 계약 주석(@state/@pre/@post/@modifies) + 선언
├── uart_driver.c               ← REQUIRE/ENSURE/check_invariants 포함 구현
├── test_uart_driver.c          ← 상태 전이 속성 테스트
├── test_contract_violation.c   ← 계약 위반 실험
├── Makefile
├── uart_driver.o
├── hal_stub.o
├── test_uart_driver.o
└── test_runner
```

---

## 실습 체크리스트

### STEP 1: 하드웨어 이해
- [ ] THR, RHR, LSR, LCR 레지스터의 역할을 설명할 수 있다
- [ ] 송신 전 LSR.THRE 확인이 필요한 이유를 설명할 수 있다

### STEP 2: 상태 기계
- [ ] 4개 상태(UNINITIALIZED, INITIALIZED, OPEN, ERROR)를 정의했다
- [ ] 상태 전이 다이어그램이 완성되었다
- [ ] 각 상태에서 호출 가능한 함수가 명확하다

### STEP 3: 인터페이스
- [ ] 6개 함수 시그니처와 에러 코드 6개가 결정되었다
- [ ] uart_config_t, uart_dev_t 구조체 필드가 결정되었다

### STEP 4: 형식적 명세
- [ ] uart_init()에 @state/@pre/@post/@modifies가 있다
- [ ] uart_open()/close()에 상태 전이 사후조건이 있다
- [ ] uart_write()/read()에 @modifies 레지스터가 명세되어 있다
- [ ] 불변식 5개 이상이 정의되었다

### STEP 5: 계약 코드
- [ ] uart_driver.h의 모든 함수에 계약 주석이 있다
- [ ] 레지스터 오프셋 매크로가 정의되어 있다

### STEP 6: 테스트 (RED)
- [ ] 10개 이상의 속성을 도출했다
- [ ] make test가 RED(링크 에러)다

### STEP 7: HAL 스텁
- [ ] hal_stub_reset()으로 테스트 격리가 가능하다
- [ ] hal_stub_set_lsr()로 LSR 상태를 제어할 수 있다

### STEP 8: 구현 (GREEN)
- [ ] 모든 @pre가 REQUIRE()로 구현되어 있다
- [ ] 모든 @post가 ENSURE()로 구현되어 있다
- [ ] check_invariants()가 각 함수 끝에서 호출된다
- [ ] make test가 ALL PASSED다

### STEP 9: 계약 위반 실험
- [ ] init 없이 open 시 REQUIRE 메시지와 abort를 확인했다
- [ ] 위반 메시지에 조건식, 파일명, 라인 번호가 포함되어 있다

---

## 핵심 규칙 요약

| 단계 | 하는 일 | 하면 안 되는 일 |
|------|---------|----------------|
| **하드웨어 이해** | 레지스터 동작 파악 | 코드 작성 |
| **상태 기계** | 상태 전이 다이어그램 | 구현, 명세 |
| **인터페이스** | API 목록, 에러 코드 | 구현 |
| **형식적 명세** | @state/@pre/@post/@modifies | 코드 작성 |
| **계약 코드** | 헤더 + 계약 주석 | uart_driver.c 작성 |
| **테스트** | 상태 전이 속성 테스트 | 구현 코드 |
| **HAL 스텁** | fake_regs 기반 스텁 | 실제 레지스터 접근 |
| **구현** | REQUIRE/ENSURE/HAL 사용 | 직접 레지스터 접근 |

> **UART 드라이버 SDD의 핵심:**
> 상태 기계를 명세하면 "잘못된 순서로 호출"이라는 가장 찾기 어려운 버그를 계약으로 방지할 수 있다.
> HAL로 하드웨어를 추상화하면 하드웨어 없이 명세를 100% 검증할 수 있다.
