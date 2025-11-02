#pragma once
#include "ecs.hpp"
#include "components.hpp"
#include "events.hpp"
#include <SDL3/SDL.h>

void MovementSystem(ecs::Registry& registry, float delta_time);

// Player Systems
void PlayerInputSystem(ecs::Registry &registry, SDL_Event &event, ecs::EventBus& evbus);
void PlayerMovementSystem(ecs::Registry& registry, float dt);
void PlayerInteractionSystem(ecs::Registry& registry);
void update_player_systems(ecs::Registry& registry, float dt);

// Render Systems
void SquareRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer);
void update_render_systems(ecs::Registry& registry, SDL_Renderer* renderer);

// Physics Systems
void AABBCollisionSystem(ecs::Registry& registry, ecs::EventBus& evbus);
void update_physics_systems(ecs::Registry& registry, ecs::EventBus& evbus);