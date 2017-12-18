#pragma once

#include <stdint.h>
#include "Building.h"

#define INPUT_LEFT 1
#define INPUT_RIGHT 2
#define INPUT_UP 4
#define INPUT_DOWN 8
#define INPUT_A 16
#define INPUT_B 32

enum BrushTypes
{
	Bulldozer,
	RoadBrush,
	PowerlineBrush,
	FirstBuildingBrush,
	LastBuildingBrush = FirstBuildingBrush + Num_BuildingTypes - 1
};

uint8_t GetInput();

void ProcessInput(void);