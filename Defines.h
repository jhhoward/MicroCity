#pragma once

#if _WIN32
#define PROGMEM
#define PSTR
#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))
#define pgm_read_ptr(x) (*((uintptr_t*)x))
#endif

#define TILE_SIZE 8
#define TILE_SIZE_SHIFT 3

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define MAP_WIDTH 48
#define MAP_HEIGHT 48

#define MAX_SCROLL_X (MAP_WIDTH * TILE_SIZE - DISPLAY_WIDTH)
#define MAX_SCROLL_Y (MAP_HEIGHT * TILE_SIZE - DISPLAY_HEIGHT)

#define VISIBLE_TILES_X ((DISPLAY_WIDTH / TILE_SIZE) + 1)
#define VISIBLE_TILES_Y ((DISPLAY_HEIGHT / TILE_SIZE) + 1)

#define MAX_BUILDINGS 128

// How long a button has to be held before the first event repeats
#define INPUT_REPEAT_TIME 10

// When repeating, how long between each event is fired
#define INPUT_REPEAT_FREQUENCY 4

#define BULLDOZER_COST 1
#define ROAD_COST 1
#define POWERLINE_COST 1
