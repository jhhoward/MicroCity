#include "Defines.h"
#include "Game.cpp"
#include "Terrain.cpp"
#include "Draw.cpp"
#include "Connectivity.cpp"
#include "Building.cpp"
#include "Interface.cpp"
#include "Simulation.cpp"

#include <stdio.h>
void PutPixel(uint8_t x, uint8_t y, uint8_t colour) {}
uint8_t GetInput() { return 0; }
int main() 
{
	printf("Building: %d bytes\n", (int)sizeof(Building));
	printf("Game state: %d bytes\n", (int)sizeof(GameState));
	return 0; 
}