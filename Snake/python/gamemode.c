/*
 * gamemode.c
 *
 * Created: 5/19/2026 1:03:44 PM
 *  Author: Lap4all
 */ 

#include <stdlib.h>
#include "gamemode.h"
#include "func.h"
#include "ws2812b.h"

/************************************************************************
 *	BIEN GAME
 */
extern uint8_t rdot, cdot;									/* Vi tri moi */
extern uint8_t nextc, nextr;								/* Vi tri tiep theo */
extern uint16_t snakeSize;									/* Do dai ran */
extern uint8_t snakeRow[64], snakeCol[64];					/* Luu than ran */
extern Direction_t direction;								/* Huong di chuyen */

static GameMode_t currentMode = GAME_MODE_WRAP;
static FoodType_t currentFood = FOOD_RED;

static uint8_t portalInRow = 0;
static uint8_t portalInCol = 0;
static uint8_t portalOutRow = MATRIX_HEIGHT - 1u;
static uint8_t portalOutCol = MATRIX_WIDTH - 1u;

#define PORTAL_NONE		0u
#define PORTAL_YELLOW	1u
#define PORTAL_BLUE		2u

static uint8_t IsSnakeCell( uint8_t row, uint8_t col )
{
	for( uint8_t i = 0; i < snakeSize; i++ ){
		if( ( snakeRow[i] == row ) && ( snakeCol[i] == col ) ){
			return 1;
		}
	}

	return 0;
}

static uint8_t IsPortalCell( uint8_t row, uint8_t col )
{
	if( currentMode != GAME_MODE_PORTAL ){
		return 0;
	}

	if( ( row == portalInRow ) && ( col == portalInCol ) ){
		return 1;
	}

	if( ( row == portalOutRow ) && ( col == portalOutCol ) ){
		return 1;
	}

	return 0;
}

static uint8_t IsFoodCellFree( uint8_t row, uint8_t col )
{
	if( IsSnakeCell(row, col) ){
		return 0;
	}

	if( IsPortalCell(row, col) ){
		return 0;
	}

	return 1;
}

static uint8_t IsOtherPortalCell( uint8_t row, uint8_t col, uint8_t movingPortal )
{
	if( currentMode != GAME_MODE_PORTAL ){
		return 0;
	}

	if( ( movingPortal != PORTAL_YELLOW ) &&
		( row == portalInRow ) &&
		( col == portalInCol ) ){
		return 1;
	}

	if( ( movingPortal != PORTAL_BLUE ) &&
		( row == portalOutRow ) &&
		( col == portalOutCol ) ){
		return 1;
	}

	return 0;
}

static uint8_t IsPortalPlacementFree( uint8_t row, uint8_t col, uint8_t movingPortal )
{
	if( IsSnakeCell(row, col) ){
		return 0;
	}

	if( ( row == rdot ) && ( col == cdot ) ){
		return 0;
	}

	if( IsOtherPortalCell(row, col, movingPortal) ){
		return 0;
	}

	return 1;
}

static void SetPortalPosition( uint8_t portal, uint8_t row, uint8_t col )
{
	if( portal == PORTAL_YELLOW ){
		portalInRow = row;
		portalInCol = col;
	}
	else if( portal == PORTAL_BLUE ){
		portalOutRow = row;
		portalOutCol = col;
	}
}

static void RandomizePortal( uint8_t portal )
{
	uint8_t found = 0;

	for( uint8_t attempts = 0; attempts < 80u; attempts++ ){
		uint8_t row = (uint8_t)(rand() % MATRIX_HEIGHT);
		uint8_t col = (uint8_t)(rand() % MATRIX_WIDTH);

		if( IsPortalPlacementFree(row, col, portal) ){
			SetPortalPosition(portal, row, col);
			found = 1;
			break;
		}
	}

	if( found == 0 ){
		for( uint8_t row = 0; row < MATRIX_HEIGHT; row++ ){
			for( uint8_t col = 0; col < MATRIX_WIDTH; col++ ){
				if( IsPortalPlacementFree(row, col, portal) ){
					SetPortalPosition(portal, row, col);
					found = 1;
					break;
				}
			}

			if( found ){
				break;
			}
		}
	}
}

static void SelectFoodType( void )
{
	currentFood = (FoodType_t)(rand() % 3);
}

static uint8_t WillHitWall( void )
{
	if( currentMode != GAME_MODE_WALL ){
		return 0;
	}

	if( ( direction == up ) && ( snakeRow[0] == 0 ) ){
		return 1;
	}

	if( ( direction == down ) && ( snakeRow[0] >= (MATRIX_HEIGHT - 1u) ) ){
		return 1;
	}

	if( ( direction == left ) && ( snakeCol[0] == 0 ) ){
		return 1;
	}

	if( ( direction == right ) && ( snakeCol[0] >= (MATRIX_WIDTH - 1u) ) ){
		return 1;
	}

	return 0;
}

static uint8_t FindModeNextRow( void )
{
	uint8_t row = snakeRow[0];

	if( direction == up ){
		row = ( row == 0u ) ? (MATRIX_HEIGHT - 1u) : (uint8_t)(row - 1u);
	}
	else if( direction == down ){
		row = ( row >= (MATRIX_HEIGHT - 1u) ) ? 0u : (uint8_t)(row + 1u);
	}

	return row;
}

static uint8_t FindModeNextCol( void )
{
	uint8_t col = snakeCol[0];

	if( direction == left ){
		col = ( col == 0u ) ? (MATRIX_WIDTH - 1u) : (uint8_t)(col - 1u);
	}
	else if( direction == right ){
		col = ( col >= (MATRIX_WIDTH - 1u) ) ? 0u : (uint8_t)(col + 1u);
	}

	return col;
}

static uint8_t FoodScore( void )
{
	if( currentFood == FOOD_YELLOW ){
		return 2;
	}

	if( currentFood == FOOD_GREEN ){
		return 3;
	}

	return 1;
}

/************************************************************************
 *	CAU HINH CHE DO
 */
void GameMode_Set( GameMode_t mode )
{
	if( mode >= MODE_COUNT ){
		currentMode = GAME_MODE_WRAP;
	}
	else {
		currentMode = mode;
	}
}

GameMode_t GameMode_Get( void )
{
	return currentMode;
}

void GameMode_Start( void )
{
	if( currentMode == GAME_MODE_PORTAL ){
		portalInRow = 0;
		portalInCol = 0;
		portalOutRow = MATRIX_HEIGHT - 1u;
		portalOutCol = MATRIX_WIDTH - 1u;
		RandomizePortal(PORTAL_YELLOW);
		RandomizePortal(PORTAL_BLUE);
	}

	GameMode_GenerateFood();
}

/************************************************************************
 *	TAO THUC AN THEO CHE DO
 */
void GameMode_GenerateFood( void )
{
	uint8_t found = 0;

	SelectFoodType();

	for( uint8_t attempts = 0; attempts < 80u; attempts++ ){
		uint8_t row = (uint8_t)(rand() % MATRIX_HEIGHT);
		uint8_t col = (uint8_t)(rand() % MATRIX_WIDTH);

		if( IsFoodCellFree(row, col) ){
			rdot = row;
			cdot = col;
			found = 1;
			break;
		}
	}

	if( found == 0 ){
		for( uint8_t row = 0; row < MATRIX_HEIGHT; row++ ){
			for( uint8_t col = 0; col < MATRIX_WIDTH; col++ ){
				if( IsFoodCellFree(row, col) ){
					rdot = row;
					cdot = col;
					found = 1;
					break;
				}
			}

			if( found ){
				break;
			}
		}
	}
}

/************************************************************************
 *	CAP NHAT RAN THEO CHE DO
 */
uint8_t GameMode_StepSnake( void )
{
	uint8_t enteredPortal = PORTAL_NONE;

	if( WillHitWall() ){
		return 1;
	}

	nextr = FindModeNextRow();
	nextc = FindModeNextCol();

	if( ( currentMode == GAME_MODE_PORTAL ) &&
		( nextr == portalInRow ) &&
		( nextc == portalInCol ) ){
		nextr = portalOutRow;
		nextc = portalOutCol;
		enteredPortal = PORTAL_YELLOW;
	}
	else if( ( currentMode == GAME_MODE_PORTAL ) &&
			 ( nextr == portalOutRow ) &&
			 ( nextc == portalOutCol ) ){
		nextr = portalInRow;
		nextc = portalInCol;
		enteredPortal = PORTAL_BLUE;
	}

	if( CheckOverlap(nextr, nextc) ){
		return 1;
	}

	if( foundDot(nextr, nextc) ){
		growSnake(nextr, nextc);
		score += FoodScore();

		if( snakeSize >= LED_COUNT ){
			return 1;
		}

		GameMode_GenerateFood();
	}
	else {
		moveForward(nextr, nextc);
	}

	if( enteredPortal != PORTAL_NONE ){
		RandomizePortal(enteredPortal);
	}

	return 0;
}

/************************************************************************
 *	THONG TIN HIEN THI
 */
uint32_t GameMode_GetFoodColor( void )
{
	if( currentFood == FOOD_YELLOW ){
		return 0xFFFF00UL;
	}

	if( currentFood == FOOD_GREEN ){
		return 0x00FF00UL;
	}

	return 0xFF0000UL;
}

uint8_t GameMode_HasPortal( void )
{
	return ( currentMode == GAME_MODE_PORTAL );
}

uint8_t GameMode_GetPortalInRow( void )
{
	return portalInRow;
}

uint8_t GameMode_GetPortalInCol( void )
{
	return portalInCol;
}

uint8_t GameMode_GetPortalOutRow( void )
{
	return portalOutRow;
}

uint8_t GameMode_GetPortalOutCol( void )
{
	return portalOutCol;
}

uint32_t GameMode_GetPortalInColor( void )
{
	return 0xFFFF00UL;
}

uint32_t GameMode_GetPortalOutColor( void )
{
	return 0x0000FFUL;
}
