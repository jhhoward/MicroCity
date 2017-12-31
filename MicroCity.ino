#include <Arduboy2.h>
#include "Draw.h"
#include "Interface.h"
#include "Game.h"
#include "Simulation.h"

Arduboy2 arduboy;

uint8_t GetInput()
{
  uint8_t result = 0;
  
  if(arduboy.pressed(A_BUTTON))
  {
    result |= INPUT_A;  
  }
  if(arduboy.pressed(B_BUTTON))
  {
    result |= INPUT_B;  
  }
  if(arduboy.pressed(UP_BUTTON))
  {
    result |= INPUT_UP;  
  }
  if(arduboy.pressed(DOWN_BUTTON))
  {
    result |= INPUT_DOWN;  
  }
  if(arduboy.pressed(LEFT_BUTTON))
  {
    result |= INPUT_LEFT;  
  }
  if(arduboy.pressed(RIGHT_BUTTON))
  {
    result |= INPUT_RIGHT;  
  }

  return result;
}

void PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
  arduboy.drawPixel(x, y, colour);
}

void DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.fillRect(x, y, w, h, colour);
}

void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.drawRect(x, y, w, h, colour);
}


uint8_t* GetPowerGrid()
{
  return arduboy.getBuffer();
}

void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(30);
  ResetVisibleTileCache();
  UIState.brush = RoadBrush; //FirstBuildingBrush + 1;
  State.money = 5000000;
}

void loop()
{
  if(arduboy.nextFrame())
  {
    Simulate();
    ProcessInput();
    UpdateInterface();
    
    Draw();
    
    /*static int y = 0;
    for(int n = 0; n < 128; n++)
    {
      PutPixel(n, y, WHITE);
    }
    y++;
    if(y >= 64) y = 0;
    */
    
    arduboy.display(false);
  }
}

