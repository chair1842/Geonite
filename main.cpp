#include <iostream>
#include <chrono>
#include <print>
#include <SDL.h>
#include "systems.hpp"  // <- where updateMovement() etc. live

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::println("SDL_Init Error: {}", SDL_GetError());
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Geonite",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::println("SDL_CreateWindow Error: {}", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::println("SDL_CreateRenderer Error: {}", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	bool running = true;

	ecs::Registry registry;

	// Entity Creation
	// Player
	ecs::Entity player = registry.create();
	registry.add<Position>(player, Position{ 320.0f, 240.0f });
	registry.add<Velocity>(player, Velocity{ 0.0f, 0.0f });
	registry.add<Input>(player, Input{ false, false, false, false, false, false, false, false });
	registry.add<PlayerTag>(player, PlayerTag{});
	registry.add<SquareDisplay>(player, SquareDisplay{ 32.0f, 128, 128, 255 });

	// --- Fixed timestep setup ---
	constexpr float FIXED_DT = 1.0f / 60.0f;  // 60 updates per second
	float accumulator = 0.0f;
	auto previous = std::chrono::high_resolution_clock::now();

	// --- Main Loop ---
	while (running) {
		// --- Handle SDL Events ---
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			PlayerInputSystem(registry, event);
		}

		// --- Timing ---
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameTime = now - previous;
		previous = now;
		float deltaTime = frameTime.count();
		if (deltaTime > 0.25f) deltaTime = 0.25f;  // prevent spiral of death
		accumulator += deltaTime;

		// --- Fixed Updates (ECS, Physics, etc.) ---
		while (accumulator >= FIXED_DT) {
			MovementSystem(registry, FIXED_DT);
			update_player_systems(registry, event);
			accumulator -= FIXED_DT;
		}

		// --- Render Step ---
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		update_render_systems(registry, renderer);

		SDL_RenderPresent(renderer);
	}

	// --- Cleanup ---
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
