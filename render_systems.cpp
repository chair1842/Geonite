#include "systems.hpp"

void SquareRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer) {
	registry.view<Position, SquareDisplay>([renderer](const Position& pos, const SquareDisplay& square) {
		SDL_Rect rect;
		rect.x = static_cast<int>(pos.x);
		rect.y = static_cast<int>(pos.y);
		rect.w = square.size;
		rect.h = square.size;
		SDL_SetRenderDrawColor(renderer, square.r, square.g, square.b, 255);
		SDL_RenderFillRect(renderer, &rect);
		});
}

void RectangleRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer) {
	registry.view<Position, RectangleDisplay>([renderer](const Position& pos, const RectangleDisplay& rectDisp) {
		SDL_Rect rect;
		rect.x = static_cast<int>(pos.x);
		rect.y = static_cast<int>(pos.y);
		rect.w = static_cast<int>(rectDisp.width);
		rect.h = static_cast<int>(rectDisp.height);
		SDL_SetRenderDrawColor(renderer, rectDisp.r, rectDisp.g, rectDisp.b, 255);
		SDL_RenderFillRect(renderer, &rect);
		});
}

void update_render_systems(ecs::Registry& registry, SDL_Renderer* renderer) {
	SquareRenderSystem(registry, renderer);
	RectangleRenderSystem(registry, renderer);
}