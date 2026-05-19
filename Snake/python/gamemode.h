/*
 * gamemode.h
 *
 * Created: 5/19/2026 1:04:05 PM
 *  Author: Lap4all
 */ 


#ifndef GAMEMODE_H_
#define GAMEMODE_H_

#include <stdint.h>

#define MODE_COUNT 3u

typedef enum {
	GAME_MODE_WRAP = 0,		/* Di xuyen tuong */
	GAME_MODE_WALL,			/* Co tuong */
	GAME_MODE_PORTAL		/* Cong dich chuyen, khong co tuong */
} GameMode_t;

typedef enum {
	FOOD_RED = 0,
	FOOD_YELLOW,
	FOOD_GREEN
} FoodType_t;

void		GameMode_Set				( GameMode_t mode );
GameMode_t	GameMode_Get				( void );
void		GameMode_Start				( void );
uint8_t		GameMode_StepSnake			( void );
void		GameMode_GenerateFood		( void );
uint32_t	GameMode_GetFoodColor		( void );
uint8_t		GameMode_HasPortal			( void );
uint8_t		GameMode_GetPortalInRow		( void );
uint8_t		GameMode_GetPortalInCol		( void );
uint8_t		GameMode_GetPortalOutRow	( void );
uint8_t		GameMode_GetPortalOutCol	( void );
uint32_t	GameMode_GetPortalInColor	( void );
uint32_t	GameMode_GetPortalOutColor	( void );

#endif /* GAMEMODE_H_ */
