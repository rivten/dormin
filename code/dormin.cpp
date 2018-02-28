#pragma once

#define DATA_FOLDER(Path) "../data/" Path

struct game_state
{
	SDL_Texture* BitmapTexture;

	u8 StaticWorld[32 * 32];

	u8 TileX;
	u8 TileY;

	b8 IsInitialised;
};

internal void
CreateStaticWorld(u8* World)
{
	for(u32 WorldY = 0; WorldY < 32; ++WorldY)
	{
		for(u32 WorldX = 0; WorldX < 32; ++WorldX)
		{
			u8* WorldTile = World + (WorldX + 32 * WorldY);
			*WorldTile = 0;
			if(WorldX == 0 || WorldX == 31 ||
					WorldY == 0 || WorldY == 31)
			{
				*WorldTile = 1;
			}
		}
	}
}

internal void
RenderStaticWorld(SDL_Renderer* Renderer, u8* World)
{
	for(u32 WorldY = 0; WorldY < 32; ++WorldY)
	{
		for(u32 WorldX = 0; WorldX < 32; ++WorldX)
		{
			u8 WorldTile = World[WorldX + WorldY * 32];
			if(WorldTile == 1)
			{
				SDL_Rect WallRect = {};
				WallRect.x = WorldX * 16;
				WallRect.y = WorldY * 16 + 3;
				WallRect.w = 16;
				WallRect.h = 13;
				SDL_SetRenderDrawColor(Renderer, 125, 119, 112, 255);
				SDL_RenderFillRect(Renderer, &WallRect);
			}
		}
	}
}

internal void
GameUpdateAndRender(game_memory* Memory,
		game_input* Input, SDL_Renderer* Renderer)
{
	Assert(sizeof(game_state) <= Memory->StorageSize);
	game_state* GameState = (game_state *)Memory->Storage;

	if(!GameState->IsInitialised)
	{
		bitmap Bitmap = LoadBitmap(DATA_FOLDER("Bisasam_16x16.png"));
		GameState->BitmapTexture = SDLCreateTextureFromBitmap(Renderer, Bitmap);

		GameState->TileX = 1;
		GameState->TileY = 1;

		CreateStaticWorld(GameState->StaticWorld);

		GameState->IsInitialised = true;
	}

	v2i PlayerIntent = {};

	if(KeyPressed(Input, SCANCODE_UP) ||
			KeyPressed(Input, SCANCODE_KP_8))
	{
		--PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_DOWN) ||
			KeyPressed(Input, SCANCODE_KP_2))
	{
		++PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_LEFT) ||
			KeyPressed(Input, SCANCODE_KP_4))
	{
		--PlayerIntent.x;
	}
	if(KeyPressed(Input, SCANCODE_RIGHT) ||
			KeyPressed(Input, SCANCODE_KP_6))
	{
		++PlayerIntent.x;
	}
	if(KeyPressed(Input, SCANCODE_KP_1))
	{
		--PlayerIntent.x;
		++PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_KP_3))
	{
		++PlayerIntent.x;
		++PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_KP_7))
	{
		--PlayerIntent.x;
		--PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_KP_9))
	{
		++PlayerIntent.x;
		--PlayerIntent.y;
	}

	v2i PlayerIntendedP = V2i(GameState->TileX, GameState->TileY) + PlayerIntent;
	if(GameState->StaticWorld[PlayerIntendedP.x + 32 * PlayerIntendedP.y] == 0)
	{
		GameState->TileX = PlayerIntendedP.x;
		GameState->TileY = PlayerIntendedP.y;
	}

	// NOTE(hugo): Render
	// {
	SDL_SetRenderDrawColor(Renderer, 36, 36, 36, 255);
	SDL_RenderClear(Renderer);

	RenderStaticWorld(Renderer, GameState->StaticWorld);

	SDL_Rect AtTile = {};
	AtTile.x = 32;
	AtTile.y = 0;
	AtTile.w = 16;
	AtTile.h = 16;

	SDL_Rect DestTile = {};
	DestTile.x = 16 * GameState->TileX;
	DestTile.y = 16 * GameState->TileY;
	DestTile.w = 16;
	DestTile.h = 16;

	SDL_RenderCopy(Renderer, GameState->BitmapTexture, &AtTile, &DestTile);

	SDL_RenderPresent(Renderer);
	// }
}

