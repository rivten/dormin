
internal void
GameUpdateAndRender(game_memory* Memory,
		game_input* Input, SDL_Renderer* Renderer)
{
	// NOTE(hugo):
	// {
	SDLSetRenderDrawColorV4(Renderer, V4(1.0f, 1.0f, 1.0f, 1.0f));
	SDL_RenderClear(Renderer);

	SDL_RenderPresent(Renderer);
	// }
}

