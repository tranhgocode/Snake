#include <avr/io.h>
#include <stdlib.h>
#include "func.h"
#include "ws2812b.h"

/************************************************************************
 *	BIEN TOAN CUC
 */
/* Bien timer */
extern volatile uint16_t ms;
extern uint16_t	compare;

/* Bien game */
extern uint8_t rdot, cdot;									/* Vi tri moi */
extern uint8_t nextc, nextr;								/* Vi tri tiep theo */
extern uint16_t snakeSize;									/* Do dai ran */
extern uint8_t snakeRow[64], snakeCol[64];					/* Luu than ran */
extern Direction_t direction;								/* Huong di chuyen */

uint16_t score = 0;

/************************************************************************
 *	TRA VE THOI GIAN
 */
uint16_t millis(void)
{
    return ms;
}

/************************************************************************
 *	KHOI TAO LAI GAME
 */
void Reload ( void ){
 
	for( uint8_t i = 0 ; i < 64; i++ ) {						/* Xoa mang than ran */
		snakeRow[i] = 0; 
		snakeCol[i] = 0;
	}
 
	snakeSize = 3;												/* Khoi tao do dai */
	score = 0;
	snakeRow[0] = 4;											/* Khoi tao hang */
	snakeRow[1] = 4;
	snakeRow[2] = 4;
 
	snakeCol[0] = 2;											/* Khoi tao cot */
	snakeCol[1] = 1;
	snakeCol[2] = 0;
 
	direction = right;											/* Huong bat dau */
	generateDot();												/* Tao moi */

}

/************************************************************************
 *	KIEM TRA HUONG HOP LE
 */
uint8_t NextPossible( Direction_t d, Direction_t dnew ){
  
	if ( ( d == right ) && ( dnew == left ) )					/* Khong cho quay dau 180 do */
		return 0; 
		
	else if ( ( d == up ) && ( dnew == down ) )
		return 0;   
		
	else if ( ( d == left ) && ( dnew == right ) )
		return 0; 
		
	else if ( ( d == down ) && ( dnew == up ) )
		return 0;
		
	else
		return 1;
}

/************************************************************************
 *	TIM HANG TIEP THEO
 */
uint8_t FindNextRow ( Direction_t d ){
  
	uint8_t r = 0;
	
	r = snakeRow[0];
	
	if( d == up ) {
		if( r > 0 )
			r--;
	}
	
	else if( d == down ){
		if( r < (MATRIX_HEIGHT - 1u) )
			r++;
	}

	return r;    
}

/************************************************************************
 *	TIM COT TIEP THEO
 */
uint8_t FindNextCol ( Direction_t d ){

	uint8_t c = 0;
	c = snakeCol[0];
  
	if( d == left ){
		if( c > 0 )
			c--;
	}
	
	else if ( d == right ){
		if( c < (MATRIX_WIDTH - 1u) )
			c++;
	}
  
	return c;
}

/************************************************************************
 *	TANG DO DAI RAN
 */
void growSnake( uint8_t nr, uint8_t nc ){
 
	if( snakeSize >= LED_COUNT )
		return;
 
	for( uint8_t i = snakeSize; i > 0; i-- ){					/* Dich than va giu duoi cu */
		snakeRow[i] = snakeRow[i-1];
		snakeCol[i] = snakeCol[i-1];
	}
 
	snakeRow[0] = nr;											/* Them dau moi */
	snakeCol[0] = nc;
	snakeSize++;
}

/************************************************************************
 *	DI CHUYEN RAN
 */
void moveForward( uint8_t r, uint8_t c ){

	for( uint8_t i = (uint8_t)(snakeSize - 1); i > 0; i-- ){	/* Dich than */
		snakeRow[i] = snakeRow[i-1];
		snakeCol[i] = snakeCol[i-1];
	}
  
	snakeRow[0] = r;
	snakeCol[0] = c;
 
}

/************************************************************************
 *	TAO MOI
 */
void generateDot ( void ){
 
	uint8_t overlap;										

	if( snakeSize >= LED_COUNT ){
		rdot = 0;
		cdot = 0;
		return;
	}
	
	do{
 
		overlap = 0;											/* Gia su chua trung than ran */
		
		rdot = (uint8_t) ( rand() % MATRIX_HEIGHT );			/* Tao toa do ngau nhien */
		cdot = (uint8_t) ( rand() % MATRIX_WIDTH );
	
		for( uint8_t i = 0; i < snakeSize; i++ ){
  
			if( ( rdot == snakeRow[i] ) && ( cdot == snakeCol[i] ) ){
				overlap = 1;
				break;
			}															   
		} 
		
	} while( overlap );
}

/************************************************************************
 *	KIEM TRA AN MOI
 */
uint8_t foundDot( uint8_t r, uint8_t c ){
  
	if( ( r == rdot ) && ( c == cdot ) )						/* Kiem tra trung toa do */
		return 1; 
	else 
		return 0;
}

/************************************************************************
 *	KIEM TRA RAN CAN THAN
 */
uint8_t CheckOverlap( uint8_t r, uint8_t c ){
  
	for( uint8_t i = 0; i < snakeSize; i++ ){
  
		if( ( snakeRow[i] == r ) && ( snakeCol[i] == c ) )
			return 1; 											/* Co trung than */
	}
  
	return 0;
}
