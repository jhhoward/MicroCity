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
	LastBuildingBrush = FirstBuildingBrush + Num_BuildingTypes - 2
};

typedef struct
{
	int16_t scrollX, scrollY;		// Where on the map (in pixels) display is scrolled to
	uint8_t selectX, selectY;		// Which tile is selected
	uint8_t brush;					// What will be placed 
  uint8_t toolbarSelection;   // For when toolbar is open
  bool showingToolbar : 1;    // Whether showing the bottom toolbar for selecting brush
} UIStateStruct;

extern UIStateStruct UIState;

uint8_t GetInput();

void ProcessInput(void);
void UpdateInterface(void);

void GetBuildingBrushLocation(BuildingType buildingType, uint8_t* outX, uint8_t* outY);


