Snake WS2812B 8x8 - ATmega16/ATmega32
======================================

Mo ta
-----
Day la project game Snake cho ma tran LED WS2812B 8x8.
Project da duoc doi tu LED matrix 16 chan sang WS2812B, chi can 1 chan data.

Thong so phan cung
------------------
- Vi dieu khien: ATmega16 hoac ATmega32
- Thach anh: 8 MHz
- F_CPU: 8000000UL
- Ma tran LED: WS2812B 8x8, tong 64 LED
- Chan DATA OUT WS2812B: PD0
- Nut len: PA0
- Nut xuong: PA1
- Nut phai: PA2
- Nut trai: PA3
- Nut select/OK: PA4

Nut bam dung kieu active-low:
- Mot dau nut noi vao chan PA0..PA4
- Dau con lai noi GND
- Code da bat pull-up noi cho PORTA

Cau truc file
-------------
- python.c: vong lap chinh, doc nut, timer, cap nhat game
- func.c: logic Snake, tao moi, va cham, ve frame
- func.h: khai bao chan nut, enum huong, prototype ham
- ws2812b.c: driver WS2812B va buffer LED
- ws2812b.h: cau hinh kich thuoc ma tran va API WS2812B

Driver WS2812B
--------------
Driver dung buffer:

    static WS2812B_Color_t led_buf[LED_COUNT];

Moi frame se:
1. Xoa buffer bang WS2812B_Clear()
2. Ve ran va moi bang WS2812B_SetPixelColorXY()
3. Goi WS2812B_Show() de day du lieu ra LED

Mau trong code dung dang RGB 0xRRGGBB.
Khi gui ra WS2812B, driver gui theo thu tu GRB.

Ham WS2812B_SendByte() dung inline assembly de tao timing bit 0 va bit 1.
Khong nen sua ham nay neu chua tinh lai chu ky lenh theo F_CPU.

Mapping LED
-----------
Mapping hien tai la hang thang:

    index = y * 8 + x

Neu ma tran WS2812B cua ban han kieu zic-zac, chi can sua ham WS2812B_XY()
trong ws2812b.c. Logic game khong can doi.

Huong di chuyen
---------------
Huong ran duoc khai bao bang enum:

    typedef enum {
        up,
        down,
        left,
        right
    } Direction_t;

Game khong cho quay dau 180 do.
Vi du: dang di sang phai thi khong the doi ngay sang trai.

Luat game hien tai
------------------
- Ran bat dau voi do dai 3 LED.
- Moi mau do xuat hien ngau nhien tren ma tran.
- Dau ran mau xanh la sang hon, than ran mau xanh la dam hon.
- Khi an moi, ran dai them 1 LED.
- Khi khong an moi, ran chi di chuyen va do dai giu nguyen.
- Neu ran can than hoac cham tuong, game over.
- Game over se nhap nhay do 3 lan roi khoi tao lai game.

Timer va toc do
---------------
Timer1 duoc cau hinh CTC de tao tick 1 ms:

    F_CPU = 8 MHz
    Prescaler = 64
    OCR1A = 124

Toc do ran duoc dat trong func.h:

    #define SNAKE_MOV_TIME 300

Giam gia tri nay de ran chay nhanh hon.
Tang gia tri nay de ran chay cham hon.

Luu y phan cung
---------------
- Can noi GND chung giua vi dieu khien va nguon LED.
- Nen dung dien tro 220-470 ohm noi tiep chan data PD0.
- Nen gan tu khoang 1000 uF giua 5V va GND cua day LED.
- WS2812B can nguon du dong. 64 LED sang trang max co the can dong lon.
- Brightness mac dinh da giam trong ws2812b.c de tranh qua tai nguon.

Build
-----
Mo file python/python.atsln bang Atmel Studio.
Project hien dang cau hinh device ATmega16 trong python.cproj.
Neu dung ATmega32, doi device trong project settings sang ATmega32.

May build can co AVR-GCC/Atmel Studio.
