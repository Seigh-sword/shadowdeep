#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include "../core/vec2.hpp"
#include "../core/rect.hpp"
#include "../terminal/color.hpp"
#include "tile.hpp"

namespace shadowdeep {

constexpr int kMapW = 160;
constexpr int kMapH = 80;
constexpr int kViewportW = 80;
constexpr int kViewportH = 24;
constexpr int kFovRadius = 12;
constexpr int kMaxMapW = 200;
constexpr int kMaxMapH = 200;

enum class Biome : uint8_t {
    Stone = 0,
    Fungal = 1,
    Crystal = 2,
    Infernal = 3,
    Abyssal = 4,
    Flooded = 5,
    Frozen = 6,
    Overgrown = 7,
    Ruins = 8,
    Void = 9
};

struct Camera {
    Vec2 pos{0, 0};
    int viewW = kViewportW;
    int viewH = kViewportH;

    void centerOn(Vec2 target, int mapW, int mapH);
    Vec2 worldToScreen(Vec2 world) const;
    Vec2 screenToWorld(Vec2 screen) const;
    bool inView(Vec2 world) const;
};

struct Dungeon {
    std::array<std::array<Tile, kMapW>, kMapH> tiles{};
    std::array<std::array<bool, kMapW>, kMapH> explored{};
    std::array<std::array<bool, kMapW>, kMapH> visible{};
    std::vector<Rect> rooms;
    Vec2 stairsUp{-1, -1};
    Vec2 stairsDown{-1, -1};
    int depth = 1;
    int regionId = 0;
    Biome biome = Biome::Stone;
    int mapW = kMapW;
    int mapH = kMapH;
    Camera camera;

    Dungeon();

    void clear();
    bool inBounds(Vec2 p) const;
    Tile at(Vec2 p) const;
    void set(Vec2 p, Tile t);
    bool blocksMove(Vec2 p) const;
    bool blocksSight(Vec2 p) const;
    bool walkable(Vec2 p) const;
    bool lineVisible(Vec2 from, Vec2 to) const;
    void computeFov(Vec2 origin, int radius = kFovRadius);
    void updateCamera(Vec2 playerPos);
    Vec2 cameraOrigin() const;
    bool isInViewport(Vec2 p) const;
    std::string biomeName() const;
    Color biomeColor() const;
};

}
