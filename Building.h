#pragma once

#include <stdint.h>

enum BuildingType
{
	BuildingType_None = 0,
	Powerplant,
	Residential,
	Commercial,
	Industrial,
	PoliceDept,
	FireDept,
	Num_BuildingTypes
};

typedef struct
{
	uint8_t x : 6;
	uint8_t y : 6;
	uint8_t type : 4;
	uint8_t populationDensity : 4;
	uint8_t flags : 3;
	bool hasPower : 1;
} Building;

typedef struct
{
	uint16_t cost;
	uint8_t width;
	uint8_t height;
} BuildingInfo;

bool PlaceBuilding(uint8_t buildingType, uint8_t x, uint8_t y);
bool CanPlaceBuilding(uint8_t buildingType, uint8_t x, uint8_t y);
const BuildingInfo* GetBuildingInfo(uint8_t buildingType);
Building* GetBuilding(uint8_t x, uint8_t y);
