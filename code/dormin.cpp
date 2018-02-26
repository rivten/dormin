#pragma once

#define DATA_FOLDER(Path) "../data/" Path

struct game_state
{
	SDL_Texture* BitmapTexture;

	u8 TileX;
	u8 TileY;

	b8 IsInitialised;
};

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

		GameState->TileX = 0;
		GameState->TileY = 0;

		GameState->IsInitialised = true;
	}

	if(KeyPressed(Input, SCANCODE_UP))
	{
		--GameState->TileY;
	}
	if(KeyPressed(Input, SCANCODE_DOWN))
	{
		++GameState->TileY;
	}
	if(KeyPressed(Input, SCANCODE_LEFT))
	{
		--GameState->TileX;
	}
	if(KeyPressed(Input, SCANCODE_RIGHT))
	{
		++GameState->TileX;
	}

	// NOTE(hugo): Render
	// {
	SDL_SetRenderDrawColor(Renderer, 36, 36, 36, 255);
	SDL_RenderClear(Renderer);

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

