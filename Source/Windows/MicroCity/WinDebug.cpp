#include <SDL.h>
#include "Defines.h"
#include "Game.h"

#define DEBUG_ZOOM_SCALE 5

SDL_Window* DebugWindow;
SDL_Renderer* DebugRenderer;
SDL_Surface* DebugSurface;
SDL_Texture* DebugTexture;

void DebugPutPixel(int x, int y, Uint32 col)
{
	SDL_Surface* surface = DebugSurface;

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	*(Uint32 *)p = col;
}

typedef struct
{
	int score;
	int crime;
	int pollution;
	int localInfluence;
	int populationEffect;
	int randomEffect;
} BuildingDebug;

const char* DebugViewNames[] =
{
	"Score",
	"Crime",
	"Pollution",
	"Local influence",
	"Population effect",
	"Random effect"
};

BuildingDebug BuildingDebugValues[MAX_BUILDINGS];

int CurrentDebugView = 0;

void DebugBuildingScore(Building* building, int score, int crime, int pollution, int localInfluence, int populationEffect, int randomEffect)
{
	for (int n = 0; n < MAX_BUILDINGS; n++)
	{
		if (building == &State.buildings[n])
		{
			BuildingDebugValues[n].score = score;
			BuildingDebugValues[n].crime = crime;
			BuildingDebugValues[n].pollution = pollution;
			BuildingDebugValues[n].localInfluence = localInfluence;
			BuildingDebugValues[n].populationEffect = populationEffect;
			BuildingDebugValues[n].randomEffect = randomEffect;
			return;
		}
	}
}

void SetCurrentDebugView(int index)
{
	CurrentDebugView = index;
	SDL_SetWindowTitle(DebugWindow, DebugViewNames[index]);
}

void UpdateDebugView()
{
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			Uint32 col = SDL_MapRGBA(DebugSurface->format, 0, 0, 0, 255);

			DebugPutPixel(x, y, col);
		}
	}

	for (int n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &State.buildings[n];
		if (building->type && !IsRubble(building->type))
		{
			Uint32 col = SDL_MapRGBA(DebugSurface->format, 0, 0, 0, 255);

			int value = 0;

			switch (CurrentDebugView)
			{
			case 0:
				value = BuildingDebugValues[n].score;
				break;
			case 1:
				value = -BuildingDebugValues[n].crime;
				break;
			case 2:
				value = -BuildingDebugValues[n].pollution;
				break;
			case 3:
				value = BuildingDebugValues[n].localInfluence;
				break;
			case 4:
				value = BuildingDebugValues[n].populationEffect;
				break;
			case 5:
				value = BuildingDebugValues[n].randomEffect;
				break;
			}

			value *= 3;

			if (value > 0)
			{
				uint8_t intensity = value < 255 ? value : 255;
				col = SDL_MapRGBA(DebugSurface->format, 0, intensity, 0, 255);
			}
			else
			{
				uint8_t intensity = value > -255 ? -value : 255;
				col = SDL_MapRGBA(DebugSurface->format, intensity, 0, 0, 255);
			}

			const BuildingInfo* info = GetBuildingInfo(building->type);
			for (int i = 0; i < info->width; i++)
			{
				for (int j = 0; j < info->height; j++)
				{
					DebugPutPixel(building->x + i, building->y + j, col);
				}
			}
		}
	}

	SDL_UpdateTexture(DebugTexture, NULL, DebugSurface->pixels, DebugSurface->pitch);
	SDL_RenderCopy(DebugRenderer, DebugTexture, NULL, NULL);
	SDL_RenderPresent(DebugRenderer);

}

void CreateDebugWindow()
{
	// Debug window
	SDL_CreateWindowAndRenderer(MAP_WIDTH * DEBUG_ZOOM_SCALE, MAP_HEIGHT * DEBUG_ZOOM_SCALE, SDL_WINDOW_RESIZABLE, &DebugWindow, &DebugRenderer);
	SDL_RenderSetLogicalSize(DebugRenderer, MAP_WIDTH, MAP_HEIGHT);

	DebugSurface = SDL_CreateRGBSurface(0, MAP_WIDTH, MAP_HEIGHT, 32,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	DebugTexture = SDL_CreateTexture(DebugRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, DebugSurface->w, DebugSurface->h);

}