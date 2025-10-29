#pragma once
#include "ecs.hpp"
#include "components.hpp"
#include "events.hpp"
#include <SDL.h>

void MovementSystem(ecs::Registry& registry, float delta_time);

// Player Systems
void PlayerInputSystem(ecs::Registry &registry, SDL_Event &event);
void PlayerMovementSystem(ecs::Registry& registry);
void PlayerInteractionSystem(ecs::Registry& registry);
void update_player_systems(ecs::Registry& registry, SDL_Event& event);

// Render Systems
void SquareRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer);
void update_render_systems(ecs::Registry& registry, SDL_Renderer* renderer);