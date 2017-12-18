#include "Game.h"
#include "Connectivity.h"
#include "Building.h"

void PowerFloodFill(uint8_t x, uint8_t y);

uint8_t GetConnections(int x, int y)
{
	if(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
	{
		int index = y * MAP_WIDTH + x;
		uint8_t mapVal = State.connectionMap[index >> 2];
		int shift = 2 * (index & 3);
		return (mapVal >> shift) & 3;
	}
	
	return 0;
}

void SetConnections(int x, int y, uint8_t newVal)
{
	if(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
	{
		int index = y * MAP_WIDTH + x;
		int shift = 2 * (index & 3);

		State.connectionMap[index >> 2] |= (newVal << shift);
	}
}

// Based on neighbouring tile types, get which visual tile to use
int GetConnectivityTileVariant(int x, int y, uint8_t mask)
{
	int variant = 0;
	
	if(y > 0 && GetConnections(x, y - 1) & mask)
	{
		variant |= 1;
	}
	if(x < MAP_WIDTH - 1 && GetConnections(x + 1, y) & mask)
	{
		variant |= 2;
	}
	if(y < MAP_HEIGHT - 1 && GetConnections(x, y + 1) & mask)
	{
		variant |= 4;
	}
	if(x > 0 && GetConnections(x - 1, y) & mask)
	{
		variant |= 8;
	}
	
	return variant;
}

// A 1 bit per tile representation of which tiles are powered
uint8_t PowerGrid[MAP_WIDTH * MAP_HEIGHT / 8];

bool IsTilePowered(uint8_t x, uint8_t y)
{
	int index = y * MAP_WIDTH + x;
	int mask = 1 << (index & 7);
	uint8_t val = PowerGrid[index >> 3];
	
	return (val & mask) != 0;
}

void SetTilePowered(uint8_t x, uint8_t y)
{
	int index = y * MAP_WIDTH + x;
	int mask = 1 << (index & 7);
	PowerGrid[index >> 3] |= mask;
}

void CalculatePowerConnectivity()
{
	// Clear power from grid
	for(int n = 0; n < MAP_WIDTH * MAP_HEIGHT / 8; n++)
	{
		PowerGrid[n] = 0;
	}

	// Flood fill from power plants
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		if(State.buildings[n].type == Powerplant)
		{
			PowerFloodFill(State.buildings[n].x, State.buildings[n].y);
		}
	}
	
	// Set powered flags on buildings
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		if(State.buildings[n].type)
		{
			State.buildings[n].hasPower = IsTilePowered(State.buildings[n].x, State.buildings[n].y);
		}
	}
}

// Power flood fill method is based on the Wikipedia 'Fixed memory method (right hand fill method)'
enum
{
	FILL_NORTH = 0,
	FILL_NORTHEAST,
	FILL_EAST,
	FILL_SOUTHEAST,
	FILL_SOUTH,
	FILL_SOUTHWEST,
	FILL_WEST,
	FILL_NORTHWEST
};

// If there is a powerline / building on this tile that hasn't been powered yet
bool IsFillEmpty(uint8_t x, uint8_t y)
{
	return (GetConnections(x, y) & PowerlineMask) != 0 && !IsTilePowered(x, y);
}

uint8_t GetFilledNeighbourCount(uint8_t x, uint8_t y)
{
	uint8_t count = 0;
	
	if(x == 0 || !IsFillEmpty(x - 1, y))
		count++;
	if(x == MAP_WIDTH - 1 || !IsFillEmpty(x + 1, y))
		count++;
	if(y == 0 || !IsFillEmpty(x, y - 1))
		count++;
	if(y == MAP_HEIGHT - 1 || !IsFillEmpty(x, y + 1))
		count++;
	
	return count;
}

bool IsFilledInDir(uint8_t x, uint8_t y, uint8_t dir)
{
	switch(dir)
	{
		default:
		case FILL_NORTH:
		return y > 0 ? !IsFillEmpty(x, y - 1) : true;
		case FILL_NORTHEAST:
		return y > 0 && x < MAP_WIDTH - 1 ? !IsFillEmpty(x + 1, y - 1) : true;
		case FILL_EAST:
		return x < MAP_WIDTH - 1 ? !IsFillEmpty(x + 1, y) : true;
		case FILL_SOUTHEAST:
		return x < MAP_WIDTH - 1 && y < MAP_HEIGHT - 1 ? !IsFillEmpty(x + 1, y + 1) : true;
		case FILL_SOUTH:
		return y < MAP_HEIGHT - 1 ? !IsFillEmpty(x, y + 1) : true;
		case FILL_SOUTHWEST:
		return x > 0 && y < MAP_HEIGHT - 1 ? !IsFillEmpty(x - 1, y + 1) : true;
		case FILL_WEST:
		return x > 0 ? !IsFillEmpty(x - 1, y) : true;
		case FILL_NORTHWEST:
		return x > 0 && y > 0 ? !IsFillEmpty(x - 1, y - 1) : true;
	}
}

uint8_t FillFrontLeft(uint8_t dir)
{
	return (dir - 1) & 7;
}

uint8_t FillBackLeft(uint8_t dir)
{
	return (dir - 3) & 7;
}

uint8_t FillTurnLeft(uint8_t dir)
{
	return (dir - 2) & 7;
}

uint8_t FillTurnRight(uint8_t dir)
{
	return (dir + 2) & 7;
}

uint8_t FillTurnAround(uint8_t dir)
{
	return (dir + 4) & 7;
}

void FillMoveForward(uint8_t* x, uint8_t* y, uint8_t dir)
{
	switch(dir)
	{
		case FILL_NORTH:
		--(*y);
		break;
		case FILL_EAST:
		++(*x);
		break;
		case FILL_SOUTH:
		++(*y);
		break;
		case FILL_WEST:
		--(*x);
		break;
	}
}

void PowerFloodFill(uint8_t x, uint8_t y)
{
	uint8_t fillDir = FILL_NORTH;
	uint8_t mark1X = 0xff, mark1Y = 0xff, mark1Dir = FILL_NORTH;
	uint8_t mark2X = 0xff, mark2Y = 0xff, mark2Dir = FILL_NORTH;
	bool mark1Set = false, mark2Set = false;
	bool backtrack = false;
	bool findloop = false;
	
	// Move to edge
	while(y > 0 && IsFillEmpty(x, y - 1))
	{
		y--;
	}
	
	goto Fill_Start;
	
	while(1)
	{
		FillMoveForward(&x, &y, fillDir);
		
		// If right pixel is empty
		if(IsFilledInDir(x, y, FillTurnRight(fillDir)) == false)
		{
			if(backtrack && !findloop &&
			(IsFilledInDir(x, y, fillDir) == false 
			|| IsFilledInDir(x, y, FillTurnLeft(fillDir)) == false))
			{
				findloop = true;
			}
			
			// Turn right
			fillDir = FillTurnRight(fillDir);

	Fill_Paint:
			
			FillMoveForward(&x, &y, fillDir);
		}
		
	Fill_Start:
		uint8_t filledNeighbourCount = GetFilledNeighbourCount(x, y);
		if(filledNeighbourCount == 4)
		{
			SetTilePowered(x, y);
			break;
		}
		
		do
		{
			fillDir = FillTurnRight(fillDir);
		}
		while(IsFilledInDir(x, y, fillDir) == false);
		do
		{
			fillDir = FillTurnLeft(fillDir);
		}
		while(IsFilledInDir(x, y, fillDir) != false);
		
		switch(filledNeighbourCount)
		{
			case 1:
				if(backtrack)
				{
					findloop = true;
				}
				else if(findloop)
				{
					mark1Set = true;
				}
				else if(IsFilledInDir(x, y, FillFrontLeft(fillDir)) == false
				&& IsFilledInDir(x, y, FillBackLeft(fillDir)) == false)
				{
					mark1Set = false;
					SetTilePowered(x, y);
					goto Fill_Paint;
				}
			break;
			case 2:
				if(IsFilledInDir(x, y, FillTurnAround(fillDir)) != false)
				{
					if(IsFilledInDir(x, y, FillFrontLeft(fillDir)) == false)
					{
						mark1Set = false;
						SetTilePowered(x, y);
						goto Fill_Paint;
					}
				}
				else if(!mark1Set)
				{
					mark1X = x;
					mark1Y = y;
					mark1Set = true;
					mark1Dir = fillDir;
					mark2Set = false;
					findloop = false;
					backtrack = false;
				}
				else
				{
					if(!mark2Set)
					{
						if(mark1Set && x == mark1X && y == mark1Y)
						{
							if(fillDir == mark1Dir)
							{
								mark1Set = false;
								fillDir = FillTurnAround(fillDir);
								SetTilePowered(x, y);
								goto Fill_Paint;
							}
							else
							{
								backtrack = true;
								findloop = false;
								fillDir = mark1Dir;
							}
						}
						else if(findloop)
						{
							mark2X = x;
							mark2Y = y;
							mark2Dir = fillDir;
							mark2Set = true;
						}
					}
					else
					{
						if(mark1Set && x == mark1X && y == mark1Y)
						{
							x = mark2X;
							y = mark2Y;
							fillDir = mark2Dir;
							mark1Set = false;
							mark2Set = false;
							backtrack = false;
							fillDir = FillTurnAround(fillDir);
							SetTilePowered(x, y);
							goto Fill_Paint;
						}
						else if(mark2Set && x == mark2X && y == mark2Y)
						{
							mark1X = x;
							mark1Y = y;
							mark1Set = true;
							fillDir = mark2Dir;
							mark1Dir = mark2Dir;
							mark2Set = false;
						}
					}
				}
			break;
			case 3:
				mark1Set = false;
				SetTilePowered(x, y);
				goto Fill_Paint;
			break;
			default:
			break;
		}
	}
}

