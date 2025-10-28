#include <iostream>
#include <print>
#include <SDL.h>

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::println("SDL_Init Error: {}", SDL_GetError());
		return 1;
	}
	SDL_Window* win = SDL_CreateWindow("Geonite", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::println("SDL_CreateWindow Error: {}", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	bool running = true;

	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	// Main loop
	while(running) {
		Uint32 frameStart = SDL_GetTicks();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}