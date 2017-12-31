#include "Game.h"

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
