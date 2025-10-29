#pragma once
#include "ecs.hpp"

struct QuitEvent {};
struct CollisionEvent {
	ecs::Entity entityA;
	ecs::Entity entityB;
};