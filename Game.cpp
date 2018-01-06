#include "Game.h"
#include "Draw.h"
#include "Interface.h"
#include "Simulation.h"

GameState State;

uint16_t GetRandFromSeed(uint16_t randVal)
{
  uint16_t lsb = randVal & 1;
  randVal >>= 1;
  if (lsb == 1)
    randVal ^= 0xB400u;

  return randVal;
}

uint16_t GetRand()
{
  static uint16_t randVal = 0xABC;

  randVal = GetRandFromSeed(randVal);

  return randVal - 1;
}

void InitGame()
{
  uint8_t* ptr = (uint8_t*)(&State);
  for(int n = 0; n < sizeof(GameState); n++)
  {
    *ptr = 0;
    ptr++;
  }
  
  ResetVisibleTileCache();
  UIState.brush = RoadBrush; //FirstBuildingBrush + 1;
  UIState.selectX = MAP_WIDTH / 2;
  UIState.selectY = MAP_HEIGHT / 2;
  UIState.scrollX = UIState.selectX * 8 + TILE_SIZE / 2 - DISPLAY_WIDTH / 2;
  UIState.scrollY = UIState.selectY * 8 + TILE_SIZE / 2 - DISPLAY_HEIGHT / 2;

  State.money = 5000000;
}

void TickGame()
{
  if(UIState.state == InGame || UIState.state == ShowingToolbar)
  {
    Simulate();
  }
  
  ProcessInput();
  UpdateInterface();
    
  Draw();
}

