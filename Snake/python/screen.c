/*
 * screen.c
 *
 * Created: 5/19/2026 1:03:24 PM
 *  Author: Lap4all
 */ 

#include "screen.h"
#include "func.h"
#include "gamemode.h"
#include "ws2812b.h"

/************************************************************************
 *	BIEN GAME CAN DE HIEN THI
 */
extern uint8_t rdot, cdot;									/* Vi tri moi */
extern uint16_t snakeSize;									/* Do dai ran */
extern uint8_t snakeRow[64], snakeCol[64];					/* Luu than ran */

static const uint8_t startIcon[8] = {
	0x30, 0x38, 0x3c, 0x3e, 0x3c, 0x38, 0x30, 0x00
};

static const uint8_t digits[10][8] = {
	{0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}, /* 0 */
	{0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, /* 1 */
	{0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00}, /* 2 */
	{0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}, /* 3 */
	{0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00}, /* 4 */
	{0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, /* 5 */
	{0x3C, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x3C, 0x00}, /* 6 */
	{0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00}, /* 7 */
	{0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00}, /* 8 */
	{0x3C, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C, 0x00}  /* 9 */
};

static const uint32_t modeColors[MODE_COUNT] = {
	0x00FF00UL,
	0xFF0000UL,
	0xFFFF00UL
};

static void DrawBitmapAt( const uint8_t bitmap[8], int8_t xOffset, WS2812B_Color_t color )
{
	for( uint8_t y = 0; y < MATRIX_HEIGHT; y++ ){
		uint8_t row = bitmap[y];

		for( uint8_t x = 0; x < MATRIX_WIDTH; x++ ){
			int8_t drawX = (int8_t)xOffset + (int8_t)x;

			if( ( row & (uint8_t)(0x80u >> x) ) &&
				( drawX >= 0 ) &&
				( drawX < (int8_t)MATRIX_WIDTH ) ){
				WS2812B_SetPixelColorXY( (uint8_t)drawX, y, color );
			}
		}
	}
}

/************************************************************************
 *	VE RAN LEN WS2812B
 */
void RenderSnake ( void ){

	WS2812B_Color_t headColor = WS2812B_HexToColor(0x00FF00UL);
	WS2812B_Color_t bodyColor = WS2812B_HexToColor(0x006000UL);
	WS2812B_Color_t foodColor = WS2812B_HexToColor(GameMode_GetFoodColor());

	WS2812B_Clear();

	if( GameMode_HasPortal() ){
		WS2812B_SetPixelColorXY(GameMode_GetPortalInCol(),
								 GameMode_GetPortalInRow(),
								 WS2812B_HexToColor(GameMode_GetPortalInColor()));

		WS2812B_SetPixelColorXY(GameMode_GetPortalOutCol(),
								 GameMode_GetPortalOutRow(),
								 WS2812B_HexToColor(GameMode_GetPortalOutColor()));
	}

	for( uint8_t i = 0 ; i < snakeSize; i++ ){
		WS2812B_SetPixelColorXY(snakeCol[i],
								 snakeRow[i],
								 (i == 0) ? headColor : bodyColor);
	}

	WS2812B_SetPixelColorXY(cdot, rdot, foodColor);
}

/************************************************************************
 *	VE MAN HINH BAT DAU
 */
void RenderTitleScreen ( void ){

	WS2812B_Color_t startColor = WS2812B_HexToColor(0x00FF00UL);

	WS2812B_Clear();
	DrawBitmapAt(startIcon, 0, startColor);
}

/************************************************************************
 *	VE MAN HINH CHON CHE DO
 */
void RenderModeSelect ( uint8_t mode ){

	uint8_t modeIndex = (uint8_t)(mode % MODE_COUNT);
	uint8_t digit = (uint8_t)(modeIndex + 1u);
	WS2812B_Color_t digitColor = WS2812B_HexToColor(modeColors[modeIndex]);

	WS2812B_Clear();
	DrawBitmapAt(digits[digit], 0, digitColor);
}

/************************************************************************
 *	KET THUC GAME
 */
void GameOver ( void ){

	WS2812B_Color_t redColor = WS2812B_HexToColor(0xFF0000UL);

	for( uint8_t i = 0; i < 3; i++ ){
		WS2812B_Fill(redColor);
		WS2812B_Show();
		WS2812B_DelayMs(150);

		WS2812B_Clear();
		WS2812B_Show();
		WS2812B_DelayMs(150);
	}
}

/************************************************************************
 *	CUON DIEM TU PHAI SANG TRAI
 */
void ScrollScore ( uint16_t value ){

	uint8_t scoreDigits[5];
	uint8_t reverseDigits[5];
	uint8_t digitCount = 0;
	WS2812B_Color_t scoreColor = WS2812B_HexToColor(0xFFFFFFUL);

	if( value == 0 ){
		scoreDigits[digitCount++] = 0;
	}
	else {
		while( ( value > 0 ) && ( digitCount < 5 ) ){
			reverseDigits[digitCount++] = (uint8_t)(value % 10u);
			value /= 10u;
		}

		for( uint8_t i = 0; i < digitCount; i++ ){
			scoreDigits[i] = reverseDigits[digitCount - 1u - i];
		}
	}

	int16_t totalWidth = (int16_t)(digitCount * 8u);

	if( digitCount > 1u ){
		totalWidth += (int16_t)(digitCount - 1u);
	}

	for( int16_t offset = MATRIX_WIDTH; offset >= -totalWidth; offset-- ){
		WS2812B_Clear();

		for( uint8_t i = 0; i < digitCount; i++ ){
			DrawBitmapAt(digits[scoreDigits[i]], (int8_t)(offset + (int16_t)(i * 9u)), scoreColor);
		}

		WS2812B_Show();
		WS2812B_DelayMs(120);
	}

	WS2812B_Clear();
	WS2812B_Show();
}
