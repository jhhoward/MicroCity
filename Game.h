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

	uint8_t terrainType;

	Building buildings[MAX_BUILDINGS];

	uint8_t taxRate;

	uint16_t residentialPopulation;
	uint16_t industrialPopulation;
	uint16_t commercialPopulation;
} GameState;

extern GameState State;

uint16_t GetRandFromSeed(uint16_t randVal);
uint16_t GetRand();

void InitGame(void);
void TickGame(void);

void SaveCity(void);
bool LoadCity(void);

