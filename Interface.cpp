#include "Game.h"
#include "Interface.h"

UIStateStruct UIState;

static uint8_t LastInput = 0;
static uint8_t InputRepeatCounter = 0;

void UpdateInterface()
{
	// Scroll screen to center the selected tile
	int targetX = UIState.selectX * 8 + TILE_SIZE / 2 - DISPLAY_WIDTH / 2;
	int targetY = UIState.selectY * 8 + TILE_SIZE / 2 - DISPLAY_HEIGHT / 2;
	
	int diffX = UIState.scrollX - targetX;
	UIState.scrollX = targetX + (diffX >> 1);
	int diffY = UIState.scrollY - targetY;
	UIState.scrollY = targetY + (diffY >> 1);
}

void GetBuildingBrushLocation(BuildingType buildingType, uint8_t* outX, uint8_t* outY)
{
	const BuildingInfo* buildingInfo = GetBuildingInfo(buildingType);
	uint8_t width = pgm_read_byte(&buildingInfo->width);
	uint8_t height = pgm_read_byte(&buildingInfo->height);
	
	if(UIState.selectX > 0)
	{
		if(UIState.selectX - 1 > MAP_WIDTH - width)
		{
			*outX = MAP_WIDTH - width;
		}
		else
		{
			*outX = UIState.selectX - 1;
		}
	}
	else
	{
		*outX = 0;
	}

	if(UIState.selectY > 0)
	{
		if(UIState.selectY - 1 > MAP_HEIGHT - height)
		{
			*outY = MAP_HEIGHT - height;
		}
		else
		{
			*outY = UIState.selectY - 1;
		}
	}
	else
	{
		*outY = 0;
	}
}

void HandleInput(uint8_t input)
{
	if((input & INPUT_LEFT) && UIState.selectX > 0)
	{
		UIState.selectX --;
	}
	if((input & INPUT_UP) && UIState.selectY > 0)
	{
		UIState.selectY --;
	}
	if((input & INPUT_RIGHT) && UIState.selectX < MAP_WIDTH - 1)
	{
		UIState.selectX ++;
	}
	if((input & INPUT_DOWN) && UIState.selectY < MAP_HEIGHT - 1)
	{
		UIState.selectY ++;
	}
	
	if(input & INPUT_A)
	{
		if(UIState.brush == Bulldozer)
		{
			Building* building = GetBuilding(UIState.selectX, UIState.selectY);
			if(building)
			{
				const BuildingInfo* buildingInfo = GetBuildingInfo(building->type);
				uint8_t width = pgm_read_byte(&buildingInfo->width);
				uint8_t height = pgm_read_byte(&buildingInfo->height);
				int cost = width * height * BULLDOZER_COST;
				
				if(State.money >= cost)
				{
					State.money -= cost;
					
					for(int y = building->y; y < building->y + height; y++)
					{
						for(int x = building->x; x < building->x + width; x++)
						{
							SetConnections(x, y, 0);
						}
					}
					
					building->type = 0;
					
					// TODO: update graphics
				}
				else
				{
					// TODO: not enough cash
				}
			}
			else
			{
				if(GetConnections(UIState.selectX, UIState.selectY))
				{
					if(State.money >= BULLDOZER_COST)
					{
						State.money -= BULLDOZER_COST;
						
						SetConnections(UIState.selectX, UIState.selectY, 0);
						
						// TODO: update graphics
					}
					else
					{
						// TODO: not enough cash
					}
				}
				else
				{
					// TODO: nothing to bulldoze
				}
			}
		}
		else if(UIState.brush < FirstBuildingBrush)
		{
			// Is powerline or road

			Building* building = GetBuilding(UIState.selectX, UIState.selectY);
			if(building)
			{
				// TODO: can't build here
			}
			else
			{
				int cost = UIState.brush == RoadBrush ? ROAD_COST : POWERLINE_COST;
				uint8_t mask = UIState.brush == RoadBrush ? RoadMask : PowerlineMask;
				uint8_t currentConnections = GetConnections(UIState.selectX, UIState.selectY);
				
				if((currentConnections & mask) == 0)
				{
					if(State.money >= cost)
					{
						State.money -= cost;
						SetConnections(UIState.selectX, UIState.selectY, currentConnections | mask);
						// TODO: update graphics
					}
					else
					{
						// TODO: not enough cash
					}
				}
			}
		}
		else
		{
			// Is building placement
			BuildingType buildingType = (BuildingType)(UIState.brush - FirstBuildingBrush);
			const BuildingInfo* buildingInfo = GetBuildingInfo(buildingType);
			uint8_t placeX, placeY;
			GetBuildingBrushLocation(buildingType, &placeX, &placeY);
			uint16_t cost = pgm_read_word(&buildingInfo->cost);
			
			if(CanPlaceBuilding(buildingType, placeX, placeY))
			{
				if(State.money >= cost)
				{
					if(PlaceBuilding(buildingType, placeX, placeY))
					{
						State.money -= cost;
						// TODO: update graphics
					}
					else
					{
						// too many buildings
					}
				}
				else
				{
					// TODO: not enough cash
				}
			}
			else
			{
				// TODO: cannot place here, e.g. obstructed
			}
		}
	}
}

void ProcessInput()
{
	uint8_t input = GetInput();
	
	if(input != LastInput)
	{
		InputRepeatCounter = 0;
		
		uint8_t newInput = LastInput ^ input;
		
		if(newInput)
		{
			HandleInput(newInput);
		}
	}
	else
	{
		InputRepeatCounter++;
		if(InputRepeatCounter > INPUT_REPEAT_TIME)
		{
			HandleInput(LastInput);
			InputRepeatCounter -= INPUT_REPEAT_FREQUENCY;
		}
	}		
	
	LastInput = input;
}