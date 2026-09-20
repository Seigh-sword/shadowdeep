#include "shadowdeep/world/dungeon.hpp"
#include <cmath>

namespace shadowdeep {

Dungeon::Dungeon() {
    clear();
}

void Dungeon::clear() {
    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            tiles[y][x] = Tile::Wall;
            explored[y][x] = false;
            visible[y][x] = false;
        }
    }
    rooms.clear();
    stairsUp = {-1, -1};
    stairsDown = {-1, -1};
    depth = 1;
}

bool Dungeon::inBounds(Vec2 p) const {
    return p.x >= 0 && p.x < kMapW && p.y >= 0 && p.y < kMapH;
}

Tile Dungeon::at(Vec2 p) const {
    return tiles[p.y][p.x];
}

void Dungeon::set(Vec2 p, Tile t) {
    if (inBounds(p)) tiles[p.y][p.x] = t;
}

bool Dungeon::blocksMove(Vec2 p) const {
    if (!inBounds(p)) return true;
    return tileBlocksMove(at(p));
}

bool Dungeon::blocksSight(Vec2 p) const {
    if (!inBounds(p)) return true;
    return tileBlocksSight(at(p));
}

bool Dungeon::walkable(Vec2 p) const {
    return inBounds(p) && !blocksMove(p);
}

bool Dungeon::lineVisible(Vec2 from, Vec2 to) const {
    int x0 = from.x;
    int y0 = from.y;
    int x1 = to.x;
    int y1 = to.y;

    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    int x = x0;
    int y = y0;

    for (;;) {
        if (x == x1 && y == y1) return true;
        if (!(x == x0 && y == y0)) {
            Vec2 p{x, y};
            if (!inBounds(p) || blocksSight(p)) return false;
        }
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }
}

void Dungeon::computeFov(Vec2 origin, int radius) {
    for (auto& row : visible) row.fill(false);

    int r2 = radius * radius;
    for (int y = origin.y - radius; y <= origin.y + radius; ++y) {
        for (int x = origin.x - radius; x <= origin.x + radius; ++x) {
            Vec2 p{x, y};
            if (!inBounds(p)) continue;
            int dx = x - origin.x;
            int dy = y - origin.y;
            if (dx * dx + dy * dy > r2) continue;
            if (lineVisible(origin, p)) {
                visible[y][x] = true;
                explored[y][x] = true;
            }
        }
    }
    if (inBounds(origin)) {
        visible[origin.y][origin.x] = true;
        explored[origin.y][origin.x] = true;
    }
}

}
