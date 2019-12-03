#include <SDL.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include "Defines.h"
#include "Game.h"
#include "Interface.h"
#include "lodepng.h"
#include "Simulation.h"
#include "WinDebug.h"

#define ZOOM_SCALE 3
#define SAVEGAME_NAME "savedcity.cty"

SDL_Window* AppWindow;
SDL_Renderer* AppRenderer;
SDL_Surface* ScreenSurface;
SDL_Texture* ScreenTexture;

uint8_t InputMask = 0;

bool IsRecording = false;
int CurrentRecordingFrame = 0;

struct KeyMap
{
	SDL_Keycode key;
	uint8_t mask;
};

KeyMap KeyMappings[] =
{
	{ SDLK_LEFT, INPUT_LEFT },
	{ SDLK_RIGHT, INPUT_RIGHT },
	{ SDLK_UP, INPUT_UP },
	{ SDLK_DOWN, INPUT_DOWN },
	{ SDLK_z, INPUT_A },
	{ SDLK_x, INPUT_B },
};

void SaveCity()
{
	FILE* fs;

	if (fopen_s(&fs, SAVEGAME_NAME, "wb") == 0)
	{
		fwrite(&State, sizeof(GameState), 1, fs);
		fflush(fs);
		fclose(fs);
	}
}

bool LoadCity()
{
	FILE* fs;

	if (fopen_s(&fs, SAVEGAME_NAME, "rb") == 0)
	{
		fread(&State, sizeof(GameState), 1, fs);
		fclose(fs);

		if (State.timeToNextDisaster > MAX_TIME_BETWEEN_DISASTERS)
		{
			State.timeToNextDisaster = MIN_TIME_BETWEEN_DISASTERS;
		}
		return true;
	}

	return false;
}

void PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
	SDL_Surface* surface = ScreenSurface;

	Uint32 col = colour ? SDL_MapRGBA(surface->format, 255, 255, 255, 255) : SDL_MapRGBA(surface->format, 0, 0, 0, 255);

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	*(Uint32 *)p = col;
}

void DrawBitmap(const uint8_t* data, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int blockX = i / 8;
			int blockY = j / 8;
			int blocksPerWidth = w / 8;
			int blockIndex = blockY * blocksPerWidth + blockX;
			uint8_t pixels = data[blockIndex * 8 + i % 8];
			uint8_t mask = 1 << (j % 8);
			if (pixels & mask)
			{
				PutPixel(x + i, y + j, 1);
			}
		}
	}
}

uint8_t GetInput()
{
	return InputMask;
}

uint8_t* GetPowerGrid()
{
	static uint8_t PowerGrid[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
	return PowerGrid;
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	CreateDebugWindow();

	SDL_CreateWindowAndRenderer(DISPLAY_WIDTH * ZOOM_SCALE, DISPLAY_HEIGHT * ZOOM_SCALE, SDL_WINDOW_RESIZABLE, &AppWindow, &AppRenderer);
	SDL_RenderSetLogicalSize(AppRenderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	ScreenSurface = SDL_CreateRGBSurface(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	ScreenTexture = SDL_CreateTexture(AppRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, ScreenSurface->w, ScreenSurface->h);

	InitGame();
	
	bool running = true;
	int playRate = 1;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				for (int n = 0; n < sizeof(KeyMappings); n++)
				{
					if (event.key.keysym.sym == KeyMappings[n].key)
					{
						InputMask |= KeyMappings[n].mask;
					}
				}
				switch (event.key.keysym.sym)
				{
				case SDLK_f:
					StartRandomFire();
					break;
				case SDLK_F1:
					SaveCity();
					break;
				case SDLK_F2:
					LoadCity();
					break;
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_TAB:
					playRate = 10;
					break;
				case SDLK_F12:
					{
						lodepng::encode(std::string("screenshot.png"), (unsigned char*)(ScreenSurface->pixels), ScreenSurface->w, ScreenSurface->h);
					}
					break;
				case SDLK_F11:
					IsRecording = !IsRecording;
					break;
				case SDLK_1:
					SetCurrentDebugView(0);
					break;
				case SDLK_2:
					SetCurrentDebugView(1);
					break;
				case SDLK_3:
					SetCurrentDebugView(2);
					break;
				case SDLK_4:
					SetCurrentDebugView(3);
					break;
				case SDLK_5:
					SetCurrentDebugView(4);
					break;
				case SDLK_6:
					SetCurrentDebugView(5);
					break;
				}
				break;
			case SDL_KEYUP:
				for (int n = 0; n < sizeof(KeyMappings); n++)
				{
					if (event.key.keysym.sym == KeyMappings[n].key)
					{
						InputMask &= ~KeyMappings[n].mask;
					}
				}
				if (event.key.keysym.sym == SDLK_TAB)
					playRate = 1;
				break;
			}
		}

		SDL_SetRenderDrawColor(AppRenderer, 206, 221, 231, 255);
		SDL_RenderClear(AppRenderer);

		//memset(ScreenSurface->pixels, 0, ScreenSurface->format->BytesPerPixel * ScreenSurface->w * ScreenSurface->h);

		for (int n = 0; n < playRate; n++)
		{
			TickGame();
		}

		if (IsRecording)
		{
			std::ostringstream filename;
			filename << "Frame";
			filename << std::setfill('0') << std::setw(5) << CurrentRecordingFrame << ".png";

			lodepng::encode(filename.str(), (unsigned char*)(ScreenSurface->pixels), ScreenSurface->w, ScreenSurface->h);
			CurrentRecordingFrame++;
		}

		SDL_UpdateTexture(ScreenTexture, NULL, ScreenSurface->pixels, ScreenSurface->pitch);
		SDL_Rect src, dest;
		src.x = src.y = dest.x = dest.y = 0;
		src.w = DISPLAY_WIDTH;
		src.h = DISPLAY_HEIGHT;
		dest.w = DISPLAY_WIDTH;
		dest.h = DISPLAY_HEIGHT;
		SDL_RenderCopy(AppRenderer, ScreenTexture, &src, &dest);
		SDL_RenderPresent(AppRenderer);

		SDL_Delay(1000 / 25);

		UpdateDebugView();
	}

	return 0;
}
