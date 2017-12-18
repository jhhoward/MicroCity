#pragma once

#include <stdint.h>
#include "Defines.h"
#include "Building.h"
#include "Connectivity.h"
#include "Terrain.h"

typedef struct
{
	uint16_t year;	// Starts at 1900
	uint8_t month;
	uint8_t simulationStep;
	
	int32_t money;

	// 2 bits per tile : road and power line
	uint8_t connectionMap[MAP_WIDTH * MAP_HEIGHT / 4];
	
	uint8_t terrain[MAP_TERRAIN_BLOCKS_WIDTH * MAP_TERRAIN_BLOCKS_HEIGHT];
	
	Building buildings[MAX_BUILDINGS];
} GameState;

extern GameState State;

