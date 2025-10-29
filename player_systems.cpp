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
