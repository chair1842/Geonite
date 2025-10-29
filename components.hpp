#pragma once

struct Position { float x; float y; };
struct Velocity { float x; float y; };
struct Health { int hp; };
struct Input { bool up; bool down; bool left; bool right; bool action; bool attack; bool sprint; bool quit; };
struct PlayerTag {};
struct NPCTag {};
struct SquareDisplay { float size; float r, g, b; };
struct SquareCollider { float size; };
struct RectangleDisplay { float width; float height; float r, g, b; };
struct RectangleCollider { float width; float height; };