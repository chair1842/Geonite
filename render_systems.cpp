#include "systems.hpp"

void SquareRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer) {
	registry.view<Position, SquareDisplay>([renderer](const Position& pos, const SquareDisplay& square) {
		SDL_FRect rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w = square.size;
		rect.h = square.size;
		SDL_SetRenderDrawColor(renderer, square.r, square.g, square.b, square.a);
		SDL_RenderFillRect(renderer, &rect);
		});
}

void RectangleRenderSystem(ecs::Registry& registry, SDL_Renderer* renderer) {
	registry.view<Position, RectangleDisplay>([renderer](const Position& pos, const RectangleDisplay& rectDisp) {
		SDL_FRect rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w = rectDisp.width;
		rect.h = rectDisp.height;
		SDL_SetRenderDrawColor(renderer, rectDisp.r, rectDisp.g, rectDisp.b, rectDisp.a);
		SDL_RenderFillRect(renderer, &rect);
		});
}

void update_render_systems(ecs::Registry& registry, SDL_Renderer* renderer) {
	SquareRenderSystem(registry, renderer);
	RectangleRenderSystem(registry, renderer);
}