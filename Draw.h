#pragma once

#include <stdint.h>

void PutPixel(uint8_t x, uint8_t y, uint8_t colour);
void DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour);
void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour);


void Draw(void);

void ResetVisibleTileCache(void);
