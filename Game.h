#pragma once

#include <stdint.h>
#include "Defines.h"
#include "Building.h"
#include "Connectivity.h"
#include "Terrain.h"

typedef struct
{
	int32_t money;

	// 2 bits per tile : road and power line
	uint8_t connectionMap[MAP_WIDTH * MAP_HEIGHT / 4];
	
	uint8_t terrain[MAP_TERRAIN_BLOCKS_WIDTH * MAP_TERRAIN_BLOCKS_HEIGHT];
	
	Building buildings[MAX_BUILDINGS];
} GameState;

extern GameState State;

typedef struct
{
	int16_t scrollX, scrollY;		// Where on the map (in pixels) display is scrolled to
	uint8_t selectX, selectY;		// Which tile is selected
	uint8_t brush;					// What will be placed 
} UIStateStruct;

extern UIStateStruct UIState;
