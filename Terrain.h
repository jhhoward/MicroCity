#pragma once

// Underlying terrain is organised by 8x8 tile blocks

#define TERRAIN_BLOCK_SIZE 8
#define MAP_TERRAIN_BLOCKS_WIDTH (MAP_WIDTH / TERRAIN_BLOCK_SIZE)
#define MAP_TERRAIN_BLOCKS_HEIGHT (MAP_HEIGHT / TERRAIN_BLOCK_SIZE)

uint8_t GetTerrainTile(int x, int y);