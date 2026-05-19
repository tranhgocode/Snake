/*
 * python.c
 *
 * Game Snake dung ma tran LED WS2812B 8x8.
 * Vi dieu khien: ATmega16/ATmega32, F_CPU = 8 MHz.
 * DATA OUT WS2812B: PD2.
 */ 
#define F_CPU	8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "func.h"
#include "gamemode.h"
#include "screen.h"
#include "ws2812b.h"

/************************************************************************
 *	BIEN TOAN CUC
 */
/* Bien timer */
volatile uint16_t ms = 0;
volatile uint8_t counter1 = 0;
volatile uint8_t counter2 = 0;
volatile uint8_t counter3 = 0;
volatile uint8_t counter4 = 0;
volatile uint8_t counter5 = 0;

uint16_t compare;

/* Bien game */
uint8_t rdot, cdot;										/* Vi tri moi */
uint8_t nextc, nextr;									/* Vi tri tiep theo */
uint16_t snakeSize;										/* Do dai ran */
uint8_t snakeRow[64], snakeCol[64];						/* Luu than ran */
Direction_t direction = right;							/* Huong di chuyen */
static GameState_t gameState = STATE_TITLE;
static uint8_t selectedMode = 0;
static uint8_t gameOverShown = 0;
static uint8_t buttonReady = SEL_MASK;

static void InitButtons(void)
{
	BTN_DDR &= (uint8_t)~SEL_MASK;
	BTN_PORT |= SEL_MASK;
}

static void InitTimer1(void)
{
	TCCR1A = 0x00;
	TCCR1B = ( ( 1 << WGM12 ) | (1 << CS10) | (1 << CS11) );
	TIMSK = ( 1 << OCIE1A );
	OCR1A = 124;											/* 1 ms voi F_CPU = 8 MHz, chia 64 */
}

static void ResetMoveTimer(void)
{
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
		ms = 0;
	}
}

static uint8_t ButtonPressed(uint8_t mask, volatile uint8_t *counter)
{
	if( ( BTN_PIN & mask ) != 0x00 ){
		buttonReady |= mask;
		return 0;
	}

	if( ( ( buttonReady & mask ) != 0x00 ) && ( *counter == 0 ) ){
		buttonReady &= (uint8_t)~mask;
		*counter = SEL_WAIT;
		return 1;
	}

	return 0;
}

static void ReadPlayingInput(void)
{
	if( ButtonPressed(SEL_UP, &counter1) && NextPossible( direction, up ) ){
		direction = up;
	}

	if( ButtonPressed(SEL_DOWN, &counter2) && NextPossible( direction, down ) ){
		direction = down;
	}

	if( ButtonPressed(SEL_LEFT, &counter3) && NextPossible( direction, left ) ){
		direction = left;
	}

	if( ButtonPressed(SEL_RIGHT, &counter4) && NextPossible( direction, right ) ){
		direction = right;
	}
}

static void EnterTitle(void)
{
	gameState = STATE_TITLE;
	RenderTitleScreen();
	WS2812B_Show();
}

static void EnterModeSelect(void)
{
	gameState = STATE_MODE_SEL;
	RenderModeSelect(selectedMode);
	WS2812B_Show();
}

static void EnterPlaying(void)
{
	gameState = STATE_PLAYING;
	GameMode_Set((GameMode_t)selectedMode);
	Reload();
	GameMode_Start();
	ResetMoveTimer();
	RenderSnake();
	WS2812B_Show();
}

static void EnterGameOver(void)
{
	gameState = STATE_GAMEOVER;
	gameOverShown = 0;
}

static void RunTitleState(void)
{
	if( ButtonPressed(SEL_SELECT, &counter5) ){
		EnterModeSelect();
	}
}

static void RunModeSelectState(void)
{
	uint8_t changed = 0;

	if( ButtonPressed(SEL_LEFT, &counter3) ){
		selectedMode = ( selectedMode == 0u ) ? (MODE_COUNT - 1u) : (uint8_t)(selectedMode - 1u);
		changed = 1;
	}

	if( ButtonPressed(SEL_RIGHT, &counter4) ){
		selectedMode++;
		if( selectedMode >= MODE_COUNT ){
			selectedMode = 0;
		}
		changed = 1;
	}

	if( changed ){
		RenderModeSelect(selectedMode);
		WS2812B_Show();
	}

	if( ButtonPressed(SEL_SELECT, &counter5) ){
		EnterPlaying();
	}
}

static void RunPlayingState(void)
{
	ReadPlayingInput();

	ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
		compare = millis();
	}

	if( compare >= SNAKE_MOV_TIME ) {
		if( GameMode_StepSnake() ){
			EnterGameOver();
		}
		else {
			ResetMoveTimer();
			RenderSnake();
			WS2812B_Show();
		}
	}
}

static void RunGameOverState(void)
{
	if( gameOverShown == 0 ){
		GameOver();
		ScrollScore(score);
		gameOverShown = 1;
		buttonReady &= (uint8_t)~SEL_SELECT;
	}

	if( ButtonPressed(SEL_SELECT, &counter5) ){
		EnterTitle();
	}
}

 /************************************************************************
  *	HAM CHINH
  */
int main(void)
{
	WS2812B_Init();
	InitButtons();
	InitTimer1();
	sei();												/* Cho phep ngat */

	EnterTitle();
	
	/* Vong lap chinh */
	while (1){
		switch( gameState ){
			case STATE_TITLE:
				RunTitleState();
				break;

			case STATE_MODE_SEL:
				RunModeSelectState();
				break;

			case STATE_PLAYING:
				RunPlayingState();
				break;

			case STATE_GAMEOVER:
				RunGameOverState();
				break;

			default:
				EnterTitle();
				break;
		}
		
	}

	return 0;
}

/************************************************************************
 *	NGAT TIMER
 */
ISR(TIMER1_COMPA_vect)
{
	ms++;  
	
	if( counter1 > 0 )
		counter1--;
		
	if( counter2 > 0 )
		counter2--;
		
	if( counter3 > 0 )
		counter3--;
		
	if( counter4 > 0 )
		counter4--;

	if( counter5 > 0 )
		counter5--;
}

  
  
  
  
  
  
  
  
  
  
  
  


