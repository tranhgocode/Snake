#ifndef WS2812B_H_
#define WS2812B_H_

#include <stdint.h>

#define MATRIX_WIDTH 8u
#define MATRIX_HEIGHT 8u
#define LED_COUNT (MATRIX_WIDTH * MATRIX_HEIGHT)

/* Mau luu trong code theo thu tu RGB. Khi gui ra LED se doi sang GRB. */
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} WS2812B_Color_t;

void WS2812B_Init(void);
void WS2812B_DelayMs(uint16_t delay_ms);
void WS2812B_Clear(void);
void WS2812B_Show(void);
void WS2812B_Fill(WS2812B_Color_t color);
void WS2812B_SetBrightness(uint8_t brightness);
uint8_t WS2812B_XY(uint8_t x, uint8_t y);
void WS2812B_SetPixelColorXY(uint8_t x, uint8_t y, WS2812B_Color_t color);
WS2812B_Color_t WS2812B_HexToColor(uint32_t color);

#endif /* WS2812B_H_ */
