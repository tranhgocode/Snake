/*
 * screen.h
 *
 * Created: 5/19/2026 1:03:06 PM
 *  Author: Lap4all
 */ 


#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

void		RenderSnake			( void );
void		RenderTitleScreen	( void );
void		RenderModeSelect	( uint8_t mode );
void		GameOver			( void );
void		ScrollScore			( uint16_t value );

#endif /* SCREEN_H_ */
