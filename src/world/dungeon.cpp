#include "shadowdeep/world/dungeon.hpp"
#include <cmath>
#include <algorithm>

namespace shadowdeep {

void Camera::centerOn(Vec2 target, int mapW, int mapH) {
    int halfW = viewW / 2;
    int halfH = viewH / 2;
    pos.x = target.x - halfW;
    pos.y = target.y - halfH;
    if (pos.x < 0) pos.x = 0;
    if (pos.y < 0) pos.y = 0;
    if (pos.x + viewW > mapW) pos.x = std::max(0, mapW - viewW);
    if (pos.y + viewH > mapH) pos.y = std::max(0, mapH - viewH);
}

Vec2 Camera::worldToScreen(Vec2 world) const {
    return {world.x - pos.x, world.y - pos.y};
}

Vec2 Camera::screenToWorld(Vec2 screen) const {
    return {screen.x + pos.x, screen.y + pos.y};
}

bool Camera::inView(Vec2 world) const {
    return world.x >= pos.x && world.x < pos.x + viewW && world.y >= pos.y && world.y < pos.y + viewH;
}

Dungeon::Dungeon() {
    clear();
}

void Dungeon::clear() {
    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Wall;
            explored[static_cast<size_t>(y)][static_cast<size_t>(x)] = false;
            visible[static_cast<size_t>(y)][static_cast<size_t>(x)] = false;
        }
    }
    rooms.clear();
    stairsUp = {-1, -1};
    stairsDown = {-1, -1};
    depth = 1;
    mapW = kMapW;
    mapH = kMapH;
    camera.pos = {0, 0};
    camera.viewW = kViewportW;
    camera.viewH = kViewportH;
    biome = Biome::Stone;
}

bool Dungeon::inBounds(Vec2 p) const {
    return p.x >= 0 && p.x < mapW && p.y >= 0 && p.y < mapH;
}

Tile Dungeon::at(Vec2 p) const {
    if (!inBounds(p)) return Tile::Wall;
    return tiles[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)];
}

void Dungeon::set(Vec2 p, Tile t) {
    if (inBounds(p)) tiles[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)] = t;
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
                visible[static_cast<size_t>(y)][static_cast<size_t>(x)] = true;
                explored[static_cast<size_t>(y)][static_cast<size_t>(x)] = true;
            }
        }
    }
    if (inBounds(origin)) {
        visible[static_cast<size_t>(origin.y)][static_cast<size_t>(origin.x)] = true;
        explored[static_cast<size_t>(origin.y)][static_cast<size_t>(origin.x)] = true;
    }
}

void Dungeon::updateCamera(Vec2 playerPos) {
    camera.centerOn(playerPos, mapW, mapH);
}

Vec2 Dungeon::cameraOrigin() const {
    return camera.pos;
}

bool Dungeon::isInViewport(Vec2 p) const {
    return camera.inView(p);
}

std::string Dungeon::biomeName() const {
    switch (biome) {
        case Biome::Stone: return "Stone Depths";
        case Biome::Fungal: return "Fungal Bloom";
        case Biome::Crystal: return "Crystal Caverns";
        case Biome::Infernal: return "Infernal Foundry";
        case Biome::Abyssal: return "Abyssal Temple";
        case Biome::Flooded: return "Flooded Halls";
        case Biome::Frozen: return "Frozen Vault";
        case Biome::Overgrown: return "Overgrown Ruins";
        case Biome::Ruins: return "Ancient Ruins";
        case Biome::Void: return "Void Tear";
        default: return "Unknown";
    }
}

Color Dungeon::biomeColor() const {
    switch (biome) {
        case Biome::Stone: return Color::Gray;
        case Biome::Fungal: return Color::Green;
        case Biome::Crystal: return Color::BrightCyan;
        case Biome::Infernal: return Color::BrightRed;
        case Biome::Abyssal: return Color::Purple;
        case Biome::Flooded: return Color::BrightCyan;
        case Biome::Frozen: return Color::BrightWhite;
        case Biome::Overgrown: return Color::BrightGreen;
        case Biome::Ruins: return Color::Brown;
        case Biome::Void: return Color::BrightMagenta;
        default: return Color::White;
    }
}

}
