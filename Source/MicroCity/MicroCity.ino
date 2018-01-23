#include <Arduboy2.h>
#include <EEPROM.h>
#include "Draw.h"
#include "Interface.h"
#include "Game.h"
#include "Simulation.h"

Arduboy2Base arduboy;

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

/*
void DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.fillRect(x, y, w, h, colour);
}

void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.drawRect(x, y, w, h, colour);
}
*/

void DrawBitmap(const uint8_t* bmp, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  arduboy.drawBitmap(x, y, bmp, w, h, WHITE);
}

void SaveCity()
{
  uint16_t address = EEPROM_STORAGE_SPACE_START;

  // Add a header so we know that the EEPROM contains a saved city
  EEPROM.update(address++, 'C'); 
  EEPROM.update(address++, 'T'); 
  EEPROM.update(address++, 'Y'); 
  EEPROM.update(address++, '1'); 

  uint8_t* ptr = (uint8_t*) &State;
  for(size_t n = 0; n < sizeof(GameState); n++)
  {
    EEPROM.update(address++, *ptr);
    ptr++;
  }
}

bool LoadCity()
{
  uint16_t address = EEPROM_STORAGE_SPACE_START;

  if(EEPROM.read(address++) != 'C') return false;
  if(EEPROM.read(address++) != 'T') return false;
  if(EEPROM.read(address++) != 'Y') return false;
  if(EEPROM.read(address++) != '1') return false;

  uint8_t* ptr = (uint8_t*) &State;
  for(size_t n = 0; n < sizeof(GameState); n++)
  {
    *ptr = EEPROM.read(address++);
    ptr++;
  }

  return true;
}

uint8_t* GetPowerGrid()
{
  return arduboy.getBuffer();
}

void setup()
{
  arduboy.boot();
  arduboy.flashlight();
  arduboy.systemButtons();
  arduboy.bootLogo();
  arduboy.setFrameRate(25);

  InitGame();
}

void loop()
{
  if(arduboy.nextFrame())
  {
    TickGame();
    arduboy.display(false);
  }
}

