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
				WallRect.x = WorldX * GlobalTileSize;
				WallRect.y = (WorldY + 2) * GlobalTileSize + 3;
				WallRect.w = GlobalTileSize;
				WallRect.h = GlobalTileSize - 3;
				SDL_SetRenderDrawColor(Renderer, 125, 119, 112, 255);
				SDL_RenderFillRect(Renderer, &WallRect);
			}
		}
	}
}

internal void
SDLWriteText(SDL_Renderer* Renderer,
		SDL_Texture* BitmapTexture, char* Str,
		u32 StartTileX, u32 StartTileY,
		v4 Color)
{
	u32 CharTileX = StartTileX;
	for(char* C = Str; *C; ++C)
	{
		u8 AsciiIndex = (u8)(*C);
		u32 BitmapTileX = AsciiIndex % 16;
		u32 BitmapTileY = AsciiIndex / 16;

		SDL_Rect SrcRect = {};
		SrcRect.x = GlobalTileSize * BitmapTileX;
		SrcRect.y = GlobalTileSize * BitmapTileY;
		SrcRect.w = GlobalTileSize;
		SrcRect.h = GlobalTileSize;

		SDL_Rect DstRect = {};
		DstRect.x = GlobalTileSize * CharTileX;
		DstRect.y = GlobalTileSize * StartTileY;
		DstRect.w = GlobalTileSize;
		DstRect.h = GlobalTileSize;

		SDLSetTextureColorMode(BitmapTexture, Color);
		SDL_RenderCopy(Renderer, BitmapTexture, &SrcRect, &DstRect);

		++CharTileX;
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

	if(KeyPressed(Input, SCANCODE_UP))
	{
		--PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_DOWN))
	{
		++PlayerIntent.y;
	}
	if(KeyPressed(Input, SCANCODE_LEFT))
	{
		--PlayerIntent.x;
	}
	if(KeyPressed(Input, SCANCODE_RIGHT))
	{
		++PlayerIntent.x;
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

	SDLWriteText(Renderer, GameState->BitmapTexture, "Arrows : ", 0, 1, V4(1.0f, 1.0f, 1.0f, 1.0f));

	// NOTE(hugo): Render player
	// {
	SDL_Rect AtTile = {};
	AtTile.x = 32;
	AtTile.y = 0;
	AtTile.w = GlobalTileSize;
	AtTile.h = GlobalTileSize;

	SDL_Rect DestTile = {};
	DestTile.x = GlobalTileSize * GameState->TileX;
	DestTile.y = GlobalTileSize * (GameState->TileY + 2);
	DestTile.w = GlobalTileSize;
	DestTile.h = GlobalTileSize;

	SDLSetTextureColorMode(GameState->BitmapTexture, V4(1.0f, 1.0f, 1.0f, 1.0f));
	SDL_RenderCopy(Renderer, GameState->BitmapTexture, &AtTile, &DestTile);
	// }


	SDL_RenderPresent(Renderer);
	// }
}

