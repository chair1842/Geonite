#include "events.hpp"
#include "systems.hpp"
#include <algorithm>

void PlayerInputSystem(ecs::Registry& registry, SDL_Event& event, ecs::EventBus& evbus) {
    registry.view<PlayerTag, Input>([&event,&evbus](PlayerTag&, Input& input) {
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
				evbus.emit(QuitEvent{});
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

void PlayerMovementSystem(ecs::Registry& registry, float dt) {
    const float normal_speed = 2.0f;
    const float sprint_multiplier = 1.5f;
    const float acceleration = 10.0f;   // how fast we reach target speed
    const float deceleration = 10.0f;    // how fast we slow down when no input

    registry.view<PlayerTag, Position, Velocity, Input>(
        [&](PlayerTag&, Position& pos, Velocity& vel, const Input& input) {

            // --- determine desired velocity based on input ---
            float speed = normal_speed;
            if (input.sprint)
                speed *= sprint_multiplier;

            float targetX = 0.0f;
            float targetY = 0.0f;

            if (input.up)    targetY -= speed;
            if (input.down)  targetY += speed;
            if (input.left)  targetX -= speed;
            if (input.right) targetX += speed;

            // normalize diagonal movement
            float mag = std::sqrt(targetX * targetX + targetY * targetY);
            if (mag > 0.0f) {
                targetX /= mag;
                targetY /= mag;
                targetX *= speed;
                targetY *= speed;
            }

            // --- accelerate or decelerate toward target velocity ---
            auto lerp = [](float a, float b, float t) {
                return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
                };

            if (targetX == 0.0f && targetY == 0.0f) {
                // no input → decelerate
                vel.x = lerp(vel.x, 0.0f, deceleration * dt);
                vel.y = lerp(vel.y, 0.0f, deceleration * dt);
            }
            else {
                // input present → accelerate toward target
                vel.x = lerp(vel.x, targetX, acceleration * dt);
                vel.y = lerp(vel.y, targetY, acceleration * dt);
            }

            // --- apply movement ---
            pos.x += vel.x * dt;
            pos.y += vel.y * dt;
        });
}



void update_player_systems(ecs::Registry& registry, float dt) {
	PlayerMovementSystem(registry, dt);
	// PlayerInteractionSystem(registry); // Placeholder for future interaction logic
}