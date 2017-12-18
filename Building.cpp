#include "Game.h"
#include "Building.h"
#include "Connectivity.h"

const BuildingInfo BuildingMetaData[] PROGMEM =
{
	// None,
	{ 0, 0, 0 },
	// Powerplant,
	{ 1000, 4, 4 },
	// Residential,
	{ 100, 3, 3 },
	// Commercial,
	{ 100, 3, 3 },
	// Industrial,
	{ 100, 3, 3 },
	// PoliceDept,
	{ 250, 3, 3 },
	// FireDept,
	{ 250, 3, 3 },
};

const BuildingInfo* GetBuildingInfo(uint8_t buildingType)
{
	return &BuildingMetaData[buildingType];
}

bool PlaceBuilding(uint8_t buildingType, uint8_t x, uint8_t y)
{
	int index = 0;
	
	while(index < MAX_BUILDINGS)
	{
		if(State.buildings[index].type == 0)
		{
			break;
		}
		index++;
	}
	
	if(index == MAX_BUILDINGS)
	{
		return false;
	}
	
	Building* newBuilding = &State.buildings[index];
	newBuilding->type = buildingType;
	newBuilding->x = x;
	newBuilding->y = y;
	newBuilding->populationDensity = 0;
	newBuilding->hasPower = false;
	newBuilding->flags = 0;
	
	// Internally building space is represented as power lines to correctly flood fill etc
	const BuildingInfo* metadata = GetBuildingInfo(buildingType);
	uint8_t width = pgm_read_byte(&metadata->width);
	uint8_t height = pgm_read_byte(&metadata->height);

	for(int i = x; i < x + width; i++)
	{
		for(int j = y; j < y + height; j++)
		{
			SetConnections(i, j, PowerlineMask);
		}
	}
	
	
	return true;
}

bool CanPlaceBuilding(uint8_t buildingType, uint8_t x, uint8_t y)
{
	const BuildingInfo* metadata = GetBuildingInfo(buildingType);
	uint8_t width = pgm_read_byte(&metadata->width);
	uint8_t height = pgm_read_byte(&metadata->height);
	
	if(x + width > MAP_WIDTH)
		return false;
	if(y + height > MAP_HEIGHT)
		return false;
	
	// TODO: check terrain
	
	// Check if trying to build on top of road or powerlines
	for(int i = x; i < x + width; i++)
	{
		for(int j = y; j < y + height; j++)
		{
			if(GetConnections(i, j) != 0)
				return false;
		}
	}
	
	// Check building overlaps
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &State.buildings[n];
		
		if(building->type)
		{
			const BuildingInfo* otherMetadata = GetBuildingInfo(building->type);
			uint8_t otherWidth = pgm_read_byte(&otherMetadata->width);
			uint8_t otherHeight = pgm_read_byte(&otherMetadata->height);
			
			if(x + width > building->x && x < building->x + otherWidth
			&& y + height > building->y && y < building->y + otherHeight)
			{
				return false;
			}
		}
		
	}
	
	return true;
}

Building* GetBuilding(uint8_t x, uint8_t y)
{
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &State.buildings[n];
		
		if(building->type)
		{
			const BuildingInfo* metadata = GetBuildingInfo(building->type);
			uint8_t width = pgm_read_byte(&otherMetadata->width);
			uint8_t height = pgm_read_byte(&otherMetadata->height);
			
			if(x >= building->x && x < building->x + width && y >= building->y && y < building->y + height)
			{
				return building;
			}
		}
	}
	
	return NULL;
}
