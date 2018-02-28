#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <rivten.h>
#include <rivten_math.h>
#include <stdio.h>

#include "dormin_input.h"

#define DORMIN_DEBUG 1

typedef bool b8;

global_variable b8 GlobalRunning = true;
const global_variable u32 GlobalWindowWidth = 512;
const global_variable u32 GlobalWindowHeight = 512;

struct game_memory
{
	u64 StorageSize;
	void* Storage;
};

internal void
SDLSetRenderDrawColorV4(SDL_Renderer* Renderer, v4 V)
{
	u8 R = u8(255.0f * V.x);
	u8 G = u8(255.0f * V.y);
	u8 B = u8(255.0f * V.z);
	u8 A = u8(255.0f * V.w);
	SDL_SetRenderDrawColor(Renderer, R, G, B, A);
}

internal u32
SafeCastToU32(s32 Value)
{
	Assert(Value >= 0);
	u32 Result = u32(Value);
	return(Result);
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct bitmap
{
	u32 Width;
	u32 Height;
	u32 Depth;
	void* Data;
};

bitmap LoadBitmap(char* Filename)
{
	// TODO(hugo) : Load off an arena
	bitmap Result = {};
	s32 Depth = 0;
	s32 Width = 0;
	s32 Height = 0;
	Result.Data = stbi_load(Filename, &Width, &Height, &Depth, 4);
	Assert(Result.Data);

	Result.Width = SafeCastToU32(Width);
	Result.Height = SafeCastToU32(Height);
	Result.Depth = SafeCastToU32(Depth);
	Assert(Result.Width > 0);
	Assert(Result.Height > 0);

	return(Result);
}

void FreeBitmap(bitmap Bitmap)
{
	stbi_image_free(Bitmap.Data);
}

internal SDL_Texture*
SDLCreateTextureFromBitmap(SDL_Renderer* Renderer, bitmap Bitmap)
{
	u32 Pitch = Bitmap.Width * 4;
	u32 RMask = 0x00FF0000;
	u32 GMask = 0x0000FF00;
	u32 BMask = 0x000000FF;
	u32 AMask = 0xFF000000;

	SDL_Surface* Surface =
		SDL_CreateRGBSurfaceFrom(Bitmap.Data,
				Bitmap.Width, Bitmap.Height,
				32, Pitch,
				RMask, GMask, BMask, AMask);
	Assert(Surface);

	SDL_Texture* Result = SDL_CreateTextureFromSurface(Renderer, Surface);
	Assert(Result);

	SDL_FreeSurface(Surface);
	return(Result);
}

#include "dormin.cpp"

int main(int ArgumentCount, char** Arguments)
{
	u32 SDLInitResult = SDL_Init(SDL_INIT_EVERYTHING);
	Assert(SDLInitResult == 0);

	// TODO(hugo) : Should the window be resizable ?
	u32 WindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	SDL_Window* Window = SDL_CreateWindow("Dormin @ rivten", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			GlobalWindowWidth, GlobalWindowHeight, WindowFlags);
	Assert(Window);

	u32 RendererFlags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, RendererFlags);
	Assert(Renderer);

	game_memory GameMemory = {};
	GameMemory.StorageSize = Megabytes(512);
	GameMemory.Storage = Allocate_(GameMemory.StorageSize);

	Assert(GameMemory.Storage);

	game_input Inputs[2] = {};
	game_input* NewInput = Inputs + 0;
	game_input* OldInput = Inputs + 1;

	// NOTE(hugo) : Timing info
	u32 LastCounter = SDL_GetTicks();
	u32 MonitorRefreshHz = 60;
	SDL_DisplayMode DisplayMode = {};
	if(SDL_GetCurrentDisplayMode(0, &DisplayMode) == 0)
	{
		if(DisplayMode.refresh_rate != 0)
		{
			MonitorRefreshHz = DisplayMode.refresh_rate;
		}
	}
	float GameUpdateHz = (MonitorRefreshHz / 2.0f);
	u32 TargetMSPerFrame = (u32)(1000.0f / GameUpdateHz);

	// TODO(hugo) : Maybe a little bit ugly to do this here
	NewInput->dtForFrame = 1.0f / GameUpdateHz;
	OldInput->dtForFrame = 1.0f / GameUpdateHz;

	while(GlobalRunning)
	{
		//
		// NOTE(hugo) : Input gathering
		// {
		//
		SDL_Event Event;
		while(SDL_PollEvent(&Event))
		{
			switch(Event.type)
			{
				case SDL_QUIT:
					{
						GlobalRunning = false;
					} break;
				default:
					{
					} break;
			}
		}
		SDLGetMouseInput(&NewInput->Mouse);
		SDLGetKeyboardInput(&NewInput->Keyboard);
		//
		// }
		//

		GameUpdateAndRender(&GameMemory, NewInput, Renderer);

		// NOTE(hugo) : Framerate computation
		u32 WorkMSElapsedForFrame = SDL_GetTicks() - LastCounter;

#if DORMIN_DEBUG
		// NOTE(hugo) : Setting the window title
		char WindowTitle[128];
		sprintf(WindowTitle, "Dormin @ rivten - (%i, %i) - %i %i %i - %ims", (s32)NewInput->Mouse.P.x, (s32)NewInput->Mouse.P.y, (s32)NewInput->Mouse.Buttons[MouseButton_Left].IsDown, (s32)NewInput->Mouse.Buttons[MouseButton_Middle].IsDown, (s32)NewInput->Mouse.Buttons[MouseButton_Right].IsDown, WorkMSElapsedForFrame);
		SDL_SetWindowTitle(Window, WindowTitle);
#endif

		if(WorkMSElapsedForFrame < TargetMSPerFrame)
		{
			u32 SleepMS = TargetMSPerFrame - WorkMSElapsedForFrame;
			if(SleepMS > 0)
			{
				SDL_Delay(SleepMS);
			}
		}
		else
		{
			// TODO(hugo) : Missed framerate
		}

		// NOTE(hugo) : This must be at the very end
		LastCounter = SDL_GetTicks();

		// NOTE(hugo) : Switch inputs
		game_input* TempInput = NewInput;
		NewInput = OldInput;
		OldInput = TempInput;

		for(u32 KeyIndex = 0;
				KeyIndex < ArrayCount(NewInput->Keyboard.Buttons);
				++KeyIndex)
		{
			NewInput->Keyboard.Buttons[KeyIndex].WasDown =
				OldInput->Keyboard.Buttons[KeyIndex].IsDown;
		}

	}

	return(0);
}
