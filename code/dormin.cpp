#pragma once

#define DATA_FOLDER(Path) "../data/" Path

enum game_mode
{
	GameMode_Rogue,
	GameMode_OptionMenu,

	GameMode_Count,
};

// TODO(hugo): Could be packing into
// flags (Is top ? Is left ? 
// are independant questions)
enum direction
{
	Dir_Top,
	Dir_TopRight,
	Dir_Right,
	Dir_BottomRight,
	Dir_Bottom,
	Dir_BottomLeft,
	Dir_Left,
	Dir_TopLeft,

	Dir_Count,
};

struct player
{
	u8 TileX;
	u8 TileY;

	direction BowDir;
};

// TODO(hugo): Cache friendly
// ordering of members ?
struct game_state
{
	SDL_Texture* BitmapTexture;

	u8 StaticWorld[32 * 32];

	player Dormin;

	game_mode Mode;

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
				SDLSetRenderDrawColorV4(Renderer, ColorPalette_Beige);
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
SDLDrawRect(SDL_Renderer* Renderer,
		u32 TileX, u32 TileY,
		v4 Color, u32 HeightDisplacement = 0)
{
	SDL_Rect Rect = {};
	Rect.x = GlobalTileSize * TileX;
	Rect.y = GlobalTileSize * TileY + HeightDisplacement;
	Rect.w = GlobalTileSize;
	Rect.h = GlobalTileSize - HeightDisplacement;

	SDLSetRenderDrawColorV4(Renderer, Color);
	SDL_RenderFillRect(Renderer, &Rect);
}

internal bool
IsIntegerInClosedRange(s32 Integer, s32 Lower, s32 Higher)
{
	bool Result = (Integer >= Lower) && (Integer <= Higher);
	return(Result);
}

internal direction
GetDirectionFromV2i(v2i V)
{
	Assert(IsIntegerInClosedRange(V.x, -1, 1));
	Assert(IsIntegerInClosedRange(V.y, -1, 1));
	Assert(V.x != 0 || V.y != 0);

	direction Result = Dir_Top;

	if(V.x == -1 && V.y == -1)
	{
		Result = Dir_TopLeft;
	}
	else if(V.x == -1 && V.y == 0)
	{
		Result = Dir_Left;
	}
	else if(V.x == -1 && V.y == 1)
	{
		Result = Dir_BottomLeft;
	}
	else if(V.x == 0 && V.y == -1)
	{
		Result = Dir_Top;
	}
	else if(V.x == 0 && V.y == 1)
	{
		Result = Dir_Bottom;
	}
	else if(V.x == 1 && V.y == -1)
	{
		Result = Dir_TopRight;
	}
	else if(V.x == 1 && V.y == 0)
	{
		Result = Dir_Right;
	}
	else if(V.x == 1 && V.y == 1)
	{
		Result = Dir_BottomRight;
	}
	else
	{
		InvalidCodePath;
	}

	return(Result);
}

internal v2i
GetOffsetFromDir(direction Dir)
{
	v2i Result = {};
	switch(Dir)
	{
		case Dir_Top:
			{
				Result = V2i(0, -1);
			} break;
		case Dir_TopRight:
			{
				Result = V2i(1, -1);
			} break;
		case Dir_Right:
			{
				Result = V2i(1, 0);
			} break;
		case Dir_BottomRight:
			{
				Result = V2i(1, 1);
			} break;
		case Dir_Bottom:
			{
				Result = V2i(0, 1);
			} break;
		case Dir_BottomLeft:
			{
				Result = V2i(-1, 1);
			} break;
		case Dir_Left:
			{
				Result = V2i(-1, 0);
			} break;
		case Dir_TopLeft:
			{
				Result = V2i(-1, -1);
			} break;
		InvalidDefaultCase;
	}

	return(Result);
}

internal void
SDLRenderBitmapTileInWorld(SDL_Renderer* Renderer,
		SDL_Texture* BitmapTexture,
		v2i WorldTile, v2i BitmapTile,
		v4 Color)
{
	SDL_Rect AtTile = {};
	AtTile.x = GlobalTileSize * BitmapTile.x;
	AtTile.y = GlobalTileSize * BitmapTile.y;
	AtTile.w = GlobalTileSize;
	AtTile.h = GlobalTileSize;

	SDL_Rect DestTile = {};
	DestTile.x = GlobalTileSize * WorldTile.x;
	DestTile.y = GlobalTileSize * (WorldTile.y + 2); // NOTE(hugo): 2 is the Y world offset from UI
	DestTile.w = GlobalTileSize;
	DestTile.h = GlobalTileSize;

	SDLSetTextureColorMode(BitmapTexture, Color);
	SDL_RenderCopy(Renderer, BitmapTexture, &AtTile, &DestTile);
}

internal v2i
BowGetTextureTile(direction Dir)
{
	v2i Result = {};
	switch(Dir)
	{
		case Dir_Top:
		case Dir_Bottom:
			{
				Result = V2i(12, 7);
			} break;
		case Dir_BottomLeft:
		case Dir_TopRight:
			{
				Result = V2i(15, 2);
			} break;
		case Dir_Left:
		case Dir_Right:
			{
				Result = V2i(13, 2);
			} break;
		case Dir_TopLeft:
		case Dir_BottomRight:
			{
				Result = V2i(12, 5);
			} break;
		InvalidDefaultCase;
	}

	return(Result);
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

		GameState->Dormin.TileX = 1;
		GameState->Dormin.TileY = 1;

		CreateStaticWorld(GameState->StaticWorld);
		
		GameState->Mode = GameMode_Rogue;

		GameState->IsInitialised = true;
	}

	v2i PlayerIntent = {};

	// TODO(hugo): If I press two keys in the same frame, then I crash
	// because I can't get the bow direction.
	// So I put an else on all these so that 
	// you can do only one move per frame. But maybe we want to support
	// multiple key press per frame
	if(KeyPressed(Input, SCANCODE_UP) ||
			KeyPressed(Input, SCANCODE_KP_8))
	{
		--PlayerIntent.y;
	}
	else if(KeyPressed(Input, SCANCODE_DOWN) ||
			KeyPressed(Input, SCANCODE_KP_2))
	{
		++PlayerIntent.y;
	}
	else if(KeyPressed(Input, SCANCODE_LEFT) ||
			KeyPressed(Input, SCANCODE_KP_4))
	{
		--PlayerIntent.x;
	}
	else if(KeyPressed(Input, SCANCODE_RIGHT) ||
			KeyPressed(Input, SCANCODE_KP_6))
	{
		++PlayerIntent.x;
	}
	else if(KeyPressed(Input, SCANCODE_KP_1))
	{
		--PlayerIntent.x;
		++PlayerIntent.y;
	}
	else if(KeyPressed(Input, SCANCODE_KP_3))
	{
		++PlayerIntent.x;
		++PlayerIntent.y;
	}
	else if(KeyPressed(Input, SCANCODE_KP_7))
	{
		--PlayerIntent.x;
		--PlayerIntent.y;
	}
	else if(KeyPressed(Input, SCANCODE_KP_9))
	{
		++PlayerIntent.x;
		--PlayerIntent.y;
	}

	if(KeyPressed(Input, SCANCODE_ESCAPE))
	{
		switch(GameState->Mode)
		{
			case GameMode_Rogue:
				{
					GameState->Mode = GameMode_OptionMenu;
				} break;
			case GameMode_OptionMenu:
				{
					GameState->Mode = GameMode_Rogue;
				} break;
			InvalidDefaultCase;
		}
	}

	v2i PlayerIntendedP = V2i(GameState->Dormin.TileX, GameState->Dormin.TileY) + PlayerIntent;
	if(GameState->StaticWorld[PlayerIntendedP.x + 32 * PlayerIntendedP.y] == 0)
	{
		GameState->Dormin.TileX = PlayerIntendedP.x;
		GameState->Dormin.TileY = PlayerIntendedP.y;
		if(PlayerIntent.x != 0 || PlayerIntent.y != 0)
		{
			GameState->Dormin.BowDir = GetDirectionFromV2i(PlayerIntent);
		}
	}

	// NOTE(hugo): Render
	// {
	SDLSetRenderDrawColorV4(Renderer, ColorPalette_Black);
	SDL_RenderClear(Renderer);

	RenderStaticWorld(Renderer, GameState->StaticWorld);

	SDLWriteText(Renderer, GameState->BitmapTexture, "Arrows : ", 0, 1, ColorPalette_White);

	// NOTE(hugo): Render player
	// {
	v2i DorminWorldTile = V2i(GameState->Dormin.TileX, GameState->Dormin.TileY);
	v2i BitmapTile = V2i(2, 0);
	SDLRenderBitmapTileInWorld(Renderer, GameState->BitmapTexture,
			DorminWorldTile, BitmapTile, ColorPalette_White);
	// }

	// NOTE(hugo): Render bow
	// {
	direction BowDir = GameState->Dormin.BowDir;
	v2i BowOffset = GetOffsetFromDir(BowDir);
	v2i BowWorldTile = DorminWorldTile + BowOffset;
	v2i BowTextureTile = BowGetTextureTile(BowDir);
	SDLRenderBitmapTileInWorld(Renderer, GameState->BitmapTexture,
			BowWorldTile, BowTextureTile, ColorPalette_White);
	// }

	// NOTE(hugo): Render boss life
	for(u32 TileX = 0; TileX < GlobalWindowTileCountX; ++TileX)
	{
		SDLDrawRect(Renderer, TileX, 0, ColorPalette_Red, 3);
	}

	// NOTE(hugo): Render arrows
	// {
	u32 ArrowCount = 3;
	for(u32 ArrowIndex = 0; ArrowIndex < ArrowCount; ++ArrowIndex)
	{
		u32 TextTileX = StringLength("Arrows : ");
		u32 Spacing = 2;
		SDLDrawRect(Renderer, TextTileX + ArrowIndex * Spacing, 1,
				ColorPalette_Blue, 3);
	}
	// }

	if(GameState->Mode == GameMode_OptionMenu)
	{
		// NOTE(hugo): Render options
		// {
		SDL_Rect OptionBackgroundRect = {};
		OptionBackgroundRect.x = 100;
		OptionBackgroundRect.y = 100;
		OptionBackgroundRect.w = 64;
		OptionBackgroundRect.h = 64;

		SDL_SetRenderDrawColor(Renderer, 73, 73, 73, 255);
		SDL_RenderFillRect(Renderer, &OptionBackgroundRect);
		// }
	}

	// }


	SDL_RenderPresent(Renderer);
	// }
}

