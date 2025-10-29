#include "systems.hpp"
#include <algorithm>

struct AABB {
    float x, y;      // top-left
    float width, height;
};

// Build AABB for either collider
inline AABB getAABB(const Position& pos, const SquareCollider& col) {
    return { pos.x, pos.y, col.size, col.size };
}

inline AABB getAABB(const Position& pos, const RectangleCollider& col) {
    return { pos.x, pos.y, col.width, col.height };
}

// --- Core collision functions ---
inline bool checkAABB(const AABB& a, const AABB& b) {
    return a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y;
}

inline void resolveAABB(Position& posA, const AABB& a, const AABB& b) {
    float overlapX = std::min(a.x + a.width - b.x, b.x + b.width - a.x);
    float overlapY = std::min(a.y + a.height - b.y, b.y + b.height - a.y);

    // Separate along the smaller overlap
    if (overlapX < overlapY) {
        if (a.x < b.x)
            posA.x -= overlapX;
        else
            posA.x += overlapX;
    }
    else {
        if (a.y < b.y)
            posA.y -= overlapY;
        else
            posA.y += overlapY;
    }
}

// --- Unified AABB Collision System ---
void AABBCollisionSystem(ecs::Registry& registry, ecs::EventBus& evbus) {
    // Collect all colliders into one list for simplicity
    std::vector<std::pair<Position*, AABB>> colliders;

    // Squares
    registry.view<Position, SquareCollider>([&](Position& pos, SquareCollider& col) {
        colliders.push_back({ &pos, getAABB(pos, col) });
        });

    // Rectangles
    registry.view<Position, RectangleCollider>([&](Position& pos, RectangleCollider& col) {
        colliders.push_back({ &pos, getAABB(pos, col) });
        });

    // Pairwise check
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            auto& [posA, a] = colliders[i];
            auto& [posB, b] = colliders[j];

            if (checkAABB(a, b)) {
				evbus.emit(CollisionEvent{ static_cast<ecs::Entity>(i), static_cast<ecs::Entity>(j) });
                resolveAABB(*posA, a, b);
            }
        }
    }
}

// --- System group for physics ---
void update_physics_systems(ecs::Registry& registry, ecs::EventBus& evbus) {
    AABBCollisionSystem(registry, evbus);
}
