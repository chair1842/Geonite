#include "systems.hpp"

void PlayerInputSystem(ecs::Registry& registry, SDL_Event& event) {
    registry.view<PlayerTag, Input>([&event](PlayerTag&, Input& input) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_w:
            case SDLK_UP:
                input.up = true;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                input.down = true;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                input.left = true;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                input.right = true;
                break;
            case SDLK_ESCAPE:
                input.quit = true;
                break;
            case SDLK_RETURN:
            case SDLK_e:
            case SDLK_SPACE:
                input.action = true;
                break;
            case SDLK_j:
                input.attack = true;
                break;
            case SDLK_LSHIFT:
                input.sprint = true;
                break;
            default:
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_w:
            case SDLK_UP:
                input.up = false;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                input.down = false;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                input.left = false;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                input.right = false;
                break;
            case SDLK_ESCAPE:
                input.quit = false;
                break;
            case SDLK_RETURN:
            case SDLK_e:
            case SDLK_SPACE:
                input.action = false;
                break;
            case SDLK_j:
                input.attack = false;
                break;
            case SDLK_LSHIFT:
                input.sprint = false;
                break;
            default:
                break;
            }
            break;
        }
        });
}

void PlayerMovementSystem(ecs::Registry& registry) {
	const float normal_speed = 2.0f;
	const float sprint_multiplier = 1.5f;
	registry.view<PlayerTag, Position, Velocity, Input>([&](PlayerTag&, Position& pos, Velocity& vel, const Input& input) {
		vel.x = 0.0f;
		vel.y = 0.0f;
		float speed = normal_speed;
		if (input.sprint) {
			speed *= sprint_multiplier;
		}
		if (input.up) {
			vel.y = -speed;
		}
		if (input.down) {
			vel.y = speed;
		}
		if (input.left) {
			vel.x = -speed;
		}
		if (input.right) {
			vel.x = speed;
		}
		});
}


void update_player_systems(ecs::Registry& registry, SDL_Event& event) {
	PlayerMovementSystem(registry);
	// PlayerInteractionSystem(registry); // Placeholder for future interaction logic
}