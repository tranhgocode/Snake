#ifndef FUNC_H_
#define FUNC_H_

#include <avr/io.h>
#include <stdint.h>

/************************************************************************
 *	KHAI BAO
 */
/* Xu ly bit */
#define CHK(x,b)	(x&b)
#define CLR(x,b)	(x&=~b)
#define SET(x,b)	(x|=b)
#define TOG(a,b)	(a^=b)

/* Nut bam: muc 0 la dang nhan, pull-up noi duoc bat trong main. */
#define BTN_DDR			DDRA
#define BTN_PORT		PORTA
#define BTN_PIN			PINA

/* Chan nut bam */
#define SEL_UP			(1u << PA0)
#define SEL_DOWN		(1u << PA1)
#define SEL_RIGHT		(1u << PA2)
#define SEL_LEFT		(1u << PA3)
#define SEL_SELECT		(1u << PA4)
#define SEL_MASK		(SEL_UP | SEL_DOWN | SEL_RIGHT | SEL_LEFT | SEL_SELECT)

/* Huong di chuyen */
typedef enum {
	up,
	down,
	left,
	right
} Direction_t;

/* Trang thai game */
typedef enum {
	STATE_TITLE,      /* Man hinh bat dau */
	STATE_MODE_SEL,   /* Chon che do */
	STATE_PLAYING,    /* Dang choi */
	STATE_GAMEOVER    /* Game over */
} GameState_t;

/* Thoi gian tinh bang ms */
#define SNAKE_MOV_TIME	300
#define SEL_WAIT		120

/************************************************************************
 *	KHAI BAO HAM
 */
uint8_t		NextPossible		( Direction_t d, Direction_t dnew );
uint8_t		FindNextRow			( Direction_t d );
uint8_t		FindNextCol			( Direction_t d );
void		growSnake			( uint8_t nr, uint8_t nc );
void		moveForward			( uint8_t r, uint8_t c );
uint8_t		CheckOverlap		( uint8_t r, uint8_t c);
void		Reload				( void );
uint8_t		foundDot			( uint8_t r, uint8_t c);
void		generateDot			( void );
uint16_t	millis				( void );

extern uint16_t score;

#endif /* FUNC_H_ */
