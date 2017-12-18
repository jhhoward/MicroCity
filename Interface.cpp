#include "Game.h"
#include "Interface.h"

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