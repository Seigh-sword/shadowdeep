#pragma once
#include <array>
#include <vector>
#include "../core/vec2.hpp"
#include "../core/rect.hpp"
#include "tile.hpp"

namespace shadowdeep {

constexpr int kMapW = 80;
constexpr int kMapH = 24;
constexpr int kFovRadius = 10;

struct Dungeon {
    std::array<std::array<Tile, kMapW>, kMapH> tiles{};
    std::array<std::array<bool, kMapW>, kMapH> explored{};
    std::array<std::array<bool, kMapW>, kMapH> visible{};
    std::vector<Rect> rooms;
    Vec2 stairsUp{-1, -1};
    Vec2 stairsDown{-1, -1};
    int depth = 1;
    int regionId = 0;

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
};

}
