#include "Game.h"
#include "Draw.h"
#include "Interface.h"

const uint8_t TileImageData[] PROGMEM = { 0 };

// Currently visible tiles are cached so they don't need to be recalculated between frames
uint8_t VisibleTileCache[VISIBLE_TILES_X * VISIBLE_TILES_Y];
int8_t CachedScrollX, CachedScrollY;

const uint8_t* GetTileData(uint8_t tile)
{
	return TileImageData + (tile * 8);
}

// Calculate which visible tile to use
uint8_t CalculateTile(int x, int y)
{
	// Check out of range
	if(x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
		return 0;
	
	// First check for buildings
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &State.buildings[n];
		
		if(building->type)
		{
			if(x < building->x || y < building->y)
				continue;
			const BuildingInfo* info = GetBuildingInfo(building->type);
			uint8_t width = pgm_read_byte(&info->width);
			uint8_t height = pgm_read_byte(&info->height);
			if(x < building->x + width && y < building->y + height)
			{
				// TODO: determine building tile type here
				return 0;
			}
		}
	}
	
	// Next check for roads / powerlines
	uint8_t connections = GetConnections(x, y);
	
	if(connections == RoadMask)
	{
		int variant = GetConnectivityTileVariant(x, y, connections);
		// TODO: find road tiles and add variant
		// TODO: traffic?
		return 0;
	}
	else if(connections == PowerlineMask)
	{
		int variant = GetConnectivityTileVariant(x, y, connections);
		// TODO: find power tiles and add variant
		return 0;
	}
	else if(connections == (PowerlineMask | RoadMask))
	{
		// TODO
		return 0;
	}
	
	return GetTerrainTile(x, y);
}

inline uint8_t GetCachedTile(int x, int y)
{
	return VisibleTileCache[y * VISIBLE_TILES_X + x];
}

void ResetVisibleTileCache()
{
	CachedScrollX = UIState.scrollX >> TILE_SIZE_SHIFT;
	CachedScrollY = UIState.scrollY >> TILE_SIZE_SHIFT;
	
	for(int y = 0; y < VISIBLE_TILES_Y; y++)
	{
		for(int x = 0; x < VISIBLE_TILES_X; x++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = CalculateTile(x + CachedScrollX, y + CachedScrollY);
		}
	}
}

// TODO: optimise loop to use the system's internal display format to write out a whole byte at a time
void DrawTiles()
{
	int tileY = UIState.scrollY >> TILE_SIZE_SHIFT;
	int offsetY = TILE_SIZE - (UIState.scrollY & (TILE_SIZE - 1));
	
	for(int row = 0; row < DISPLAY_HEIGHT; row++)
	{
		int tileX = UIState.scrollX >> TILE_SIZE_SHIFT;
		int offsetX = TILE_SIZE - (UIState.scrollX & (TILE_SIZE - 1));
		uint8_t currentTile = GetCachedTile(tileX, tileY);
		uint8_t readBuf = pgm_read_byte(&GetTileData(currentTile)[offsetY]);
		readBuf >>= offsetX;
		
		for(int col = 0; col < DISPLAY_WIDTH; col++)
		{
			PutPixel(col, row, readBuf & 1);
			
			offsetX = (offsetX + 1) & 7;
			readBuf >>= 1;

			if(!offsetX)
			{
				tileX++;
				currentTile = GetCachedTile(tileX, tileY);
				readBuf = pgm_read_byte(&GetTileData(currentTile)[offsetY]);
			}
		}
		
		offsetY = (offsetY + 1) & 7;
		if(!offsetY)
		{
			tileY++;
		}
	}
}

void ScrollUp(int amount)
{
	CachedScrollY -= amount;
	int y = VISIBLE_TILES_Y - 1;
	
	for(int n = 0; n < VISIBLE_TILES_Y - amount; n++)
	{
		for(int x = 0; x < VISIBLE_TILES_X; x++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = VisibleTileCache[(y - amount) * VISIBLE_TILES_X + x];
		}
		y--;
	}
	
	for(y = 0; y < amount; y++)
	{
		for(int x = 0; x < VISIBLE_TILES_X; x++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = CalculateTile(x + CachedScrollX, y + CachedScrollY);
		}
	}
}

void ScrollDown(int amount)
{
	CachedScrollY += amount;
	int y = 0;
	
	for(int n = 0; n < VISIBLE_TILES_Y - amount; n++)
	{
		for(int x = 0; x < VISIBLE_TILES_X; x++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = VisibleTileCache[(y + amount) * VISIBLE_TILES_X + x];
		}
		y++;
	}
	
	y = VISIBLE_TILES_Y - 1;
	for(int n = 0; n < amount; n++)
	{
		for(int x = 0; x < VISIBLE_TILES_X; x++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = CalculateTile(x + CachedScrollX, y + CachedScrollY);
		}
		y--;
	}
}

void ScrollLeft(int amount)
{
	CachedScrollX -= amount;
	int x = VISIBLE_TILES_X - 1;
	
	for(int n = 0; n < VISIBLE_TILES_X - amount; n++)
	{
		for(int y = 0; y < VISIBLE_TILES_Y; y++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = VisibleTileCache[y * VISIBLE_TILES_X + x - amount];
		}
		x--;
	}
	
	for(x = 0; x < amount; x++)
	{
		for(int y = 0; y < VISIBLE_TILES_Y; y++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = CalculateTile(x + CachedScrollX, y + CachedScrollY);
		}
	}
}

void ScrollRight(int amount)
{
	CachedScrollX += amount;
	int x = 0;
	
	for(int n = 0; n < VISIBLE_TILES_X - amount; n++)
	{
		for(int y = 0; y < VISIBLE_TILES_Y; y++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = VisibleTileCache[y * VISIBLE_TILES_X + x + amount];
		}
		x++;
	}
	
	x = VISIBLE_TILES_Y - 1;
	for(int n = 0; n < amount; n++)
	{
		for(int y = 0; y < VISIBLE_TILES_Y; y++)
		{
			VisibleTileCache[y * VISIBLE_TILES_X + x] = CalculateTile(x + CachedScrollX, y + CachedScrollY);
		}
		x--;
	}
}

void DrawCursor()
{
	uint8_t cursorX, cursorY;
	int cursorWidth = TILE_SIZE, cursorHeight = TILE_SIZE;
	
	if(UIState.brush >= FirstBuildingBrush)
	{
		BuildingType buildingType = (BuildingType)(UIState.brush - FirstBuildingBrush);
		GetBuildingBrushLocation(buildingType, &cursorX, &cursorY);
		const BuildingInfo* buildingInfo = GetBuildingInfo(UIState.brush - FirstBuildingBrush);
		cursorWidth *= pgm_read_byte(&buildingInfo->width);
		cursorHeight *= pgm_read_byte(&buildingInfo->height);
	}
	else
	{
		cursorX = UIState.selectX;
		cursorY = UIState.selectY;
	}

	int cursorDrawX, cursorDrawY;
	cursorDrawX = (cursorX * 8) - UIState.scrollX;
	cursorDrawY = (cursorY * 8) - UIState.scrollY;
	
	if(cursorDrawX >= 0 && cursorDrawY >= 0 && cursorDrawX + cursorWidth < DISPLAY_WIDTH && cursorDrawY + cursorHeight < DISPLAY_HEIGHT)
	{
		static uint8_t cursorAnimation = 0;
		cursorAnimation++;
		
		for(int n = 0; n < cursorWidth; n++)
		{
			uint8_t colour = ((n + cursorAnimation) & 4) != 0 ? 1 : 0;
			PutPixel(cursorX + n, cursorY, colour);
			PutPixel(cursorX + cursorWidth - n - 1, cursorY + cursorHeight - 1, colour);
		}

		for(int n = 0; n < cursorHeight; n++)
		{
			uint8_t colour = ((n + cursorAnimation) & 4) != 0 ? 1 : 0;
			PutPixel(cursorX, cursorY + n, colour);
			PutPixel(cursorX + cursorWidth - 1, cursorY + cursorHeight - n - 1, colour);
		}
	}
}

void Draw()
{
	// Check to see if scrolled to a new location and need to update the visible tile cache
	int tileScrollX = UIState.scrollX >> TILE_SIZE_SHIFT;
	int tileScrollY = UIState.scrollY >> TILE_SIZE_SHIFT;
	int scrollDiffX = tileScrollX - CachedScrollX;
	int scrollDiffY = tileScrollY - CachedScrollY;
	
	if(scrollDiffX < 0)
	{
		if(scrollDiffX > -VISIBLE_TILES_X)
		{
			ScrollLeft(-scrollDiffX);
		}
		else
		{
			ResetVisibleTileCache();
		}
	}
	else if(scrollDiffX > 0)
	{
		if(scrollDiffX < VISIBLE_TILES_X)
		{
			ScrollRight(scrollDiffX);
		}
		else
		{
			ResetVisibleTileCache();
		}
	}

	if(scrollDiffY < 0)
	{
		if(scrollDiffY > -VISIBLE_TILES_Y)
		{
			ScrollUp(-scrollDiffY);
		}
		else
		{
			ResetVisibleTileCache();
		}
	}
	else if(scrollDiffY > 0)
	{
		if(scrollDiffY < VISIBLE_TILES_Y)
		{
			ScrollDown(scrollDiffY);
		}
		else
		{
			ResetVisibleTileCache();
		}
	}

	DrawTiles();
}
