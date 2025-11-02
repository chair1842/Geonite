#include "systems.hpp"

void MovementSystem(ecs::Registry& registry, float delta_time) {
	registry.view<Position, Velocity, Movable>([delta_time](Position& pos, const Velocity& vel, Movable&) {
		pos.x += vel.x * 100 * delta_time;
		pos.y += vel.y * 100 * delta_time;
		});
}