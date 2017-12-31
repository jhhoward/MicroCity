#include <stdint.h>
#include "Terrain.h"
#include "Game.h"

uint8_t GetTerrainTile(int x, int y)
{
	// TODO
  if(y > 45)
  {
    return FIRST_WATER_TILE + ((((y * 359)) ^ ((x * 431))) & 3);
  }
	return FIRST_TERRAIN_TILE + ((((y * 359)) ^ ((x * 431))) & 3);
}
