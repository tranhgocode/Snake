#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "ws2812b.h"

#define WS2812B_DATA_DDR DDRD
#define WS2812B_DATA_PORT PORTD
#define WS2812B_DATA_BIT 2u

#define WS2812B_DEFAULT_BRIGHTNESS 80u
#define WS2812B_RESET_US 300u
#define COLOR_MAX_VALUE 255u
#define LED_INDEX_NONE LED_COUNT

static WS2812B_Color_t led_buf[LED_COUNT];
static uint8_t ws2812b_brightness = COLOR_MAX_VALUE;

void WS2812B_DelayMs(uint16_t delay_ms)
{
	while (delay_ms-- != 0u)
	{
		_delay_ms(1);
	}
}

static inline uint8_t WS2812B_ApplyBrightness(uint8_t value)
{
	return (uint8_t)(((uint16_t)value * ws2812b_brightness) / COLOR_MAX_VALUE);
}

WS2812B_Color_t WS2812B_HexToColor(uint32_t color)
{
	WS2812B_Color_t out;

	out.r = WS2812B_ApplyBrightness((uint8_t)((color >> 16) & 0xFFu));
	out.g = WS2812B_ApplyBrightness((uint8_t)((color >> 8) & 0xFFu));
	out.b = WS2812B_ApplyBrightness((uint8_t)(color & 0xFFu));

	return out;
}

static void WS2812B_SendByte(uint8_t data)
{
	uint8_t ctr;
	uint8_t lo = WS2812B_DATA_PORT & (uint8_t)~(1u << WS2812B_DATA_BIT);
	uint8_t hi = WS2812B_DATA_PORT | (uint8_t)(1u << WS2812B_DATA_BIT);

	__asm__ volatile(
		"    ldi  %[ctr], 8           \n\t"
		"bitloop%=:                   \n\t"
		"    out  %[port], %[hi]      \n\t"
		"    nop                      \n\t"
		"    sbrs %[data], 7          \n\t"
		"    out  %[port], %[lo]      \n\t"
		"    lsl  %[data]             \n\t"
		"    nop                      \n\t"
		"    out  %[port], %[lo]      \n\t"
		"    dec  %[ctr]              \n\t"
		"    brne bitloop%=           \n\t"
		: [ctr] "=&d"(ctr),
		  [data] "+r"(data)
		: [port] "I"(_SFR_IO_ADDR(WS2812B_DATA_PORT)),
		  [hi] "r"(hi),
		  [lo] "r"(lo));
}

static inline void WS2812B_SendColor(WS2812B_Color_t color)
{
	WS2812B_SendByte(color.g);
	WS2812B_SendByte(color.r);
	WS2812B_SendByte(color.b);
}

void WS2812B_Init(void)
{
	WS2812B_DATA_DDR |= (uint8_t)(1u << WS2812B_DATA_BIT);
	WS2812B_DATA_PORT &= (uint8_t)~(1u << WS2812B_DATA_BIT);

	WS2812B_SetBrightness(WS2812B_DEFAULT_BRIGHTNESS);
	WS2812B_Clear();
	_delay_us(WS2812B_RESET_US);
	WS2812B_Show();
}

void WS2812B_Clear(void)
{
	memset(led_buf, 0, sizeof(led_buf));
}

void WS2812B_Show(void)
{
	uint8_t sreg_save = SREG;

	cli();
	for (uint8_t i = 0u; i < LED_COUNT; i++)
	{
		WS2812B_SendColor(led_buf[i]);
	}
	SREG = sreg_save;

	_delay_us(WS2812B_RESET_US);
}

void WS2812B_Fill(WS2812B_Color_t color)
{
	for (uint8_t i = 0u; i < LED_COUNT; i++)
	{
		led_buf[i] = color;
	}
}

void WS2812B_SetBrightness(uint8_t brightness)
{
	ws2812b_brightness = brightness;
}

uint8_t WS2812B_XY(uint8_t x, uint8_t y)
{
	if ((x >= MATRIX_WIDTH) || (y >= MATRIX_HEIGHT))
	{
		return LED_INDEX_NONE;
	}

	return (uint8_t)((y * MATRIX_WIDTH) + x);
}

void WS2812B_SetPixelColorXY(uint8_t x, uint8_t y, WS2812B_Color_t color)
{
	uint8_t index = WS2812B_XY(x, y);

	if (index != LED_INDEX_NONE)
	{
		led_buf[index] = color;
	}
}
