#include <iostream>
#include <chrono>
#include <print>
#include <SDL.h>
#include "systems.hpp"  // <- where updateMovement() etc. live
#include "events.hpp"

void create_initial_entities(ecs::Registry& registry) {
	ecs::Entity player = registry.create();
	registry.add<Position>(player, Position{ 320.0f - 20, 240.0f - 20 });
	registry.add<Velocity>(player, Velocity{ 0.0f, 0.0f });
	registry.add<Input>(player, Input{ false, false, false, false, false, false, false, false });
	registry.add<PlayerTag>(player, PlayerTag{});
	registry.add<SquareDisplay>(player, SquareDisplay{ 40.0f, 128, 128, 255 });
	registry.add<SquareCollider>(player, SquareCollider{ 40.0f });
	// Unmovable Box
	ecs::Entity box1 = registry.create();
	registry.add<Position>(box1, Position{ 200.0f, 200.0f });
	registry.add<SquareDisplay>(box1, SquareDisplay{ 80.0f, 255, 0, 0 });
	registry.add<SquareCollider>(box1, SquareCollider{ 80.0f });
	ecs::Entity box2 = registry.create();
	registry.add<Position>(box2, Position{ 400.0f, 300.0f });
	registry.add<SquareDisplay>(box2, SquareDisplay{ 100.0f, 0, 255, 0 });
	registry.add<SquareCollider>(box2, SquareCollider{ 100.0f });
	ecs::Entity box3 = registry.create();
	registry.add<Position>(box3, Position{ 150.0f, 350.0f });
	registry.add<RectangleDisplay>(box3, RectangleDisplay{ 120.0f, 60.0f, 0, 0, 255 });
	registry.add<RectangleCollider>(box3, RectangleCollider{ 120.0f, 60.0f });
}

void register_events(ecs::EventBus& evbus) {
	evbus.registerEvent<QuitEvent>();
	evbus.registerEvent<CollisionEvent>();
}

void quit_event(ecs::EventBus& evbus, bool& running) {
	using clock = std::chrono::steady_clock;
	static clock::time_point lastPressTime{};
	static int quitPressCount = 0;

	evbus.subscribe<QuitEvent>([&](const QuitEvent&) {
		auto now = clock::now();

		// if time between presses > 0.5s, reset
		if (quitPressCount == 0 || std::chrono::duration<float>(now - lastPressTime).count() > 0.5f) {
			quitPressCount = 1;
			std::println("Press ESC again to quit.");
		}
		else {
			std::println("Quitting...");
			running = false;
		}

		lastPressTime = now;
		});
}

void quit(SDL_Renderer* renderer, SDL_Window* window) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

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
	ecs::EventBus evbus;

	create_initial_entities(registry);
	register_events(evbus);

	quit_event(evbus, running);

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
			PlayerInputSystem(registry, event, evbus);
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
			update_player_systems(registry, FIXED_DT);
			update_physics_systems(registry, evbus);
			evbus.dispatchAllEvents();
			accumulator -= FIXED_DT;
		}

		// --- Render Step ---
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		update_render_systems(registry, renderer);

		SDL_RenderPresent(renderer);
	}

	// --- Cleanup ---
	quit(renderer, win);
	return 0;
}
