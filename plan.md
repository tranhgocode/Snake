# KẾ HOẠCH NÂNG CẤP GAME SNAKE - WS2812B 8x8
**MCU:** ATmega16/32 | **F_CPU:** 8 MHz | **LED:** WS2812B 64 LED | **Nút:** PA0-PA3 (Up/Down/Right/Left)

---

## TỔNG QUAN THAY ĐỔI

### Phần cứng bổ sung
- **Thêm 1 nút bấm (PA4):** Nút SELECT/START - dùng để xác nhận chọn chế độ và bắt đầu game
- Nút LEFT/RIGHT: chuyển chế độ trên màn hình chọn

### Flow game mới
```
Power On -> Màn hình Start (hiệu ứng LED)
    -> Nhấn START -> Màn hình chọn chế độ
        -> LEFT/RIGHT để duyệt 6 chế độ
        -> Nhấn START để bắt đầu
            -> Chơi game (tốc độ tăng dần)
                -> Game Over (hiệu ứng)
                    -> Quay về màn hình Start
```

---

## CẤU TRÚC FILE SAU KHI SỬA

| File | Vai trò |
|------|---------|
| python.c | Main loop, state machine, timer, input |
| func.h | Khai báo, enum, define, prototype |
| func.c | Logic game, render, các chế độ |
| ws2812b.c/.h | Driver LED (giữ nguyên) |

---

## PHASE 1: CƠ SỞ HẠ TẦNG

### 1.1 Thêm nút SELECT (PA4)
**File:** func.h
```c
#define SEL_START   (1u << PA4)
#define SEL_MASK    (SEL_UP | SEL_DOWN | SEL_RIGHT | SEL_LEFT | SEL_START)
```
**File:** python.c - Thêm counter5 cho debounce nút START

### 1.2 State Machine
**File:** func.h
```c
typedef enum {
    STATE_TITLE,      /* Màn hình bắt đầu */
    STATE_MODE_SEL,   /* Chọn chế độ */
    STATE_PLAYING,    /* Đang chơi */
    STATE_GAMEOVER    /* Game over */
} GameState_t;

typedef enum {
    MODE_CLASSIC_WARP,     /* 1. Bình thường + xuyên tường */
    MODE_CLASSIC_WALL,     /* 2. Không xuyên tường */
    MODE_PORTAL,           /* 3. Cổng dịch chuyển */
    MODE_GROWING_WALL,     /* 4. Tường mọc dần */
    MODE_MOVING_FOOD,      /* 5. Mồi di chuyển */
    MODE_SPECIAL_FOOD,     /* 6. Mồi đặc biệt */
    MODE_COUNT             /* = 6 */
} GameMode_t;
```

### 1.3 Hệ thống tốc độ động
**File:** func.h
```c
#define SPEED_BASE    350   /* ms - tốc độ ban đầu */
#define SPEED_MIN     80    /* ms - tốc độ tối đa */
#define SPEED_STEP    15    /* ms - giảm mỗi khi ăn mồi */
```
**File:** func.c - Hàm tính tốc độ:
```c
uint16_t GetSnakeSpeed(void) {
    uint16_t speed = SPEED_BASE - (snakeSize - 3) * SPEED_STEP;
    if (speed < SPEED_MIN) speed = SPEED_MIN;
    return speed;
}
```
Thay SNAKE_MOV_TIME bằng GetSnakeSpeed() trong vòng lặp chính

### 1.4 Hệ thống tính điểm
```c
extern uint16_t score;
```
- Mỗi mồi thường: +10 điểm
- Mồi đặc biệt (mode 6): +20/+30 tùy loại
- Điểm hiển thị khi Game Over (cuộn số trên LED)

---

## PHASE 2: MÀN HÌNH & UI

### 2.1 Màn hình Title (STATE_TITLE)
- Hiệu ứng: LED chạy xung quanh viền ma trận (border chase) bằng màu xanh lá
- Hoặc: nhấp nháy chữ "S" ở giữa ma trận
- Nhấn START -> chuyển STATE_MODE_SEL

### 2.2 Màn hình chọn chế độ (STATE_MODE_SEL)
- Hiển thị số chế độ (1-6) bằng font đơn giản ở giữa ma trận
- Viền LED đổi màu theo chế độ:
  - Mode 1: Xanh lá | Mode 2: Đỏ | Mode 3: Tím
  - Mode 4: Vàng | Mode 5: Cam | Mode 6: Xanh dương
- LEFT/RIGHT: tăng/giảm currentMode
- START: chuyển STATE_PLAYING, gọi Reload()

### 2.3 Hiệu ứng Game Over cải tiến (STATE_GAMEOVER)
- Bước 1: Rắn "nổ" - LED rắn tắt từ đuôi lên đầu (mỗi 50ms tắt 1 LED)
- Bước 2: Màn hình đỏ nhấp nháy 3 lần
- Bước 3: Cuộn điểm số (nếu đủ flash cho font số)
- Bước 4: Delay 1s -> quay về STATE_TITLE

---

## PHASE 3: HIỆU ỨNG MỒI NHẤP NHÁY

### 3.1 Blink mồi
**File:** python.c - Dùng biến blink_counter trong ISR
```c
volatile uint8_t blink_tick = 0;  /* tăng mỗi ms trong ISR */
```
**File:** func.c - Trong RenderSnake():
```c
/* Mồi nhấp nháy: sáng 200ms, tối 100ms */
uint8_t blink_phase = (blink_tick / 200) % 2;
if (blink_phase == 0) {
    WS2812B_SetPixelColorXY(cdot, rdot, foodColor);
} else {
    WS2812B_SetPixelColorXY(cdot, rdot, dimFoodColor); /* màu mờ hơn */
}
```

---

## PHASE 4: CÁC CHẾ ĐỘ CHƠI

### 4.1 Mode 1 - Bình thường + Xuyên tường
- Bỏ WillHitWall() check
- Sửa FindNextRow/FindNextCol: wrap sang phía đối diện
```c
if (d == left)  c = (c == 0) ? (MATRIX_WIDTH - 1) : (c - 1);
if (d == right) c = (c >= MATRIX_WIDTH - 1) ? 0 : (c + 1);
```

### 4.2 Mode 2 - Không xuyên tường (giống hiện tại)
- Giữ nguyên logic WillHitWall() -> Game Over

### 4.3 Mode 3 - Cổng dịch chuyển (Portal)
**Biến thêm:**
```c
uint8_t rdot2, cdot2;         /* Vị trí mồi thứ 2 */
uint8_t portal_active;        /* Rắn đang đi qua portal? */
Direction_t portal_exit_dir;  /* Hướng ra portal */
```
**Logic:**
1. generateDot() tạo 2 mồi cùng lúc (dot1 và dot2)
2. Khi đầu rắn chạm dot1:
   - Xác định hướng ăn (direction hiện tại)
   - Đặt đầu rắn tại vị trí dot2
   - Hướng ra = hướng ngược lại hướng ăn
   - Rắn tiếp tục di chuyển từ dot2 theo hướng ngược
3. Tương tự khi chạm dot2 -> dịch đến dot1
4. **Không phải teleport tức thời:** Rắn đi vào portal, thân rắn dần xuất hiện ở phía portal kia. Cách triển khai:
   - Khi ăn portal: đặt đầu mới ở vị trí portal đích
   - Thân rắn vẫn ở vị trí cũ
   - Mỗi bước di chuyển tiếp theo, thân rắn tự nhiên kéo theo
   - Tạo hiệu ứng đi qua cổng tự nhiên

### 4.4 Mode 4 - Tường mọc dần
**Biến thêm:**
```c
uint8_t wallRow[30], wallCol[30];  /* Vị trí tường (tối đa ~30) */
uint8_t wallCount;                  /* Số tường hiện tại */
uint8_t eatCount;                   /* Đếm số lần ăn */
```
**Logic:**
1. Mỗi lần ăn mồi: eatCount++
2. Khi eatCount % 2 == 0 (mỗi 2 lần ăn): tạo 1 block tường vàng (0xFFFF00)
3. Rắn chạm tường -> Game Over
4. generateDot() phải tránh cả vị trí tường

### 4.5 Mode 5 - Mồi di chuyển
**Biến thêm:**
```c
uint16_t food_move_timer;     /* Timer riêng cho mồi */
Direction_t food_direction;   /* Hướng mồi đang đi */
```
**Logic:**
1. Mồi di chuyển mỗi GetSnakeSpeed() * 2 ms (nửa tốc độ rắn)
2. Mồi đổi hướng khi chạm biên hoặc chạm thân rắn
3. Mồi KHÔNG đi xuyên qua rắn: check ô tiếp theo trước khi di chuyển
4. Nếu tất cả hướng bị chặn: mồi đứng yên
5. Mồi xuyên tường (wrap around)

### 4.6 Mode 6 - Mồi đặc biệt
**Biến thêm:**
```c
typedef enum {
    FOOD_NORMAL,    /* Đỏ - +1 size, +10 điểm */
    FOOD_GOLDEN,    /* Vàng - +2 size, +30 điểm */
    FOOD_SHRINK,    /* Xanh dương - -1 size, +5 điểm */
    FOOD_SPEED_UP,  /* Tím - tốc độ x2 trong 5s */
    FOOD_SLOW_DOWN  /* Trắng - tốc độ /2 trong 5s */
} FoodType_t;

FoodType_t currentFoodType;
uint16_t effect_timer;
uint8_t speed_modifier;       /* 0=normal, 1=fast, 2=slow */
```
**Logic:**
1. Random loại mồi (Normal 50%, Golden 10%, Shrink 15%, SpeedUp 15%, SlowDown 10%)
2. Mỗi loại mồi có màu riêng
3. Hiệu ứng tốc độ tạm thời dùng effect_timer đếm ngược
4. FOOD_SHRINK: giảm size nhưng không dưới 3

---

## PHASE 5: TÍCH HỢP MAIN LOOP

### 5.1 State Machine trong python.c
```c
while (1) {
    ReadInput();
    switch (gameState) {
        case STATE_TITLE:
            RunTitleScreen();
            break;
        case STATE_MODE_SEL:
            RunModeSelect();
            break;
        case STATE_PLAYING:
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { compare = millis(); }
            if (compare >= GetSnakeSpeed()) {
                StepSnake();
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ms = 0; }
                RenderSnake();
                WS2812B_Show();
            }
            if (currentMode == MODE_MOVING_FOOD) HandleMovingFood();
            break;
        case STATE_GAMEOVER:
            RunGameOver();
            break;
    }
}
```

---

## THỨ TỰ TRIỂN KHAI

| # | Việc | Thời gian | Phụ thuộc |
|---|------|-----------|-----------|
| 1 | Thêm nút PA4, debounce | 15 phút | Không |
| 2 | State machine + flow | 30 phút | #1 |
| 3 | Màn hình Title | 20 phút | #2 |
| 4 | Màn hình chọn chế độ | 30 phút | #2 |
| 5 | Tốc độ động theo size | 10 phút | Không |
| 6 | Hệ thống điểm | 15 phút | Không |
| 7 | Hiệu ứng mồi nhấp nháy | 15 phút | Không |
| 8 | Hiệu ứng Game Over mới | 20 phút | #2 |
| 9 | Mode 1 - Xuyên tường | 15 phút | #2 |
| 10 | Mode 2 - Tường cứng | 5 phút | Có sẵn |
| 11 | Mode 3 - Portal | 45 phút | #2 |
| 12 | Mode 4 - Tường mọc | 30 phút | #2 |
| 13 | Mode 5 - Mồi chạy | 40 phút | #2 |
| 14 | Mode 6 - Mồi đặc biệt | 35 phút | #2 |

**Tổng ước lượng: ~5-6 giờ code**

---

## LƯU Ý QUAN TRỌNG

1. **RAM ATmega16 = 1KB:** snakeRow/Col[64] = 128B, wallRow/Col[30] = 60B, led_buf[64]*3 = 192B. Tổng ~400B, vẫn OK.
2. **Flash ATmega16 = 16KB:** Font số 0-9 thêm ~80B PROGMEM. Code mở rộng nên vừa.
3. **Timer:** Dùng chung Timer1 ISR 1ms cho mọi counter.
4. **rand():** Seed bằng giá trị timer khi nhấn START lần đầu.
5. **ws2812b.c/.h:** Không cần sửa, API đủ dùng.
