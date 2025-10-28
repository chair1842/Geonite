#include "systems.hpp"

void PlayerInputSystem(ecs::Registry& registry, SDL_Event& event) {
	registry.view<PlayerTag, Input>([&event](PlayerTag&, Input& input) {
		// To be implemented
		});
}