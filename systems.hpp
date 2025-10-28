#pragma once
#include "ecs.hpp"
#include "components.hpp"
#include "events.hpp"
#include <SDL.h>

void MovementSystem(ecs::Registry &registry) {
	registry.view<Position, Velocity>([](Position& pos, const Velocity& vel) {
		pos.x += vel.x;
		pos.y += vel.y;
		});
}

// Player Systems
void PlayerInputSystem(ecs::Registry &registry, SDL_Event &event);
void PlayerMovementSystem(ecs::Registry& registry);
void PLayerInteractionSystem(ecs::Registry& registry);
