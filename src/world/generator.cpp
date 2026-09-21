#include "shadowdeep/world/generator.hpp"
#include "shadowdeep/world/region.hpp"
#include <algorithm>
#include <queue>

namespace shadowdeep {

static Biome biomeForDepth(int depth, Rng& rng) {
    if (depth >= 60) {
        int r = rng.range(0, 99);
        if (r < 40) return Biome::Void;
        if (r < 60) return Biome::Abyssal;
        if (r < 80) return Biome::Infernal;
        return Biome::Ruins;
    }
    if (depth >= 45) {
        int r = rng.range(0, 99);
        if (r < 30) return Biome::Abyssal;
        if (r < 60) return Biome::Infernal;
        if (r < 80) return Biome::Crystal;
        return Biome::Ruins;
    }
    if (depth >= 30) {
        int r = rng.range(0, 99);
        if (r < 25) return Biome::Abyssal;
        if (r < 50) return Biome::Infernal;
        if (r < 75) return Biome::Flooded;
        return Biome::Ruins;
    }
    RegionId reg = regionForDepth(depth);
    switch (reg) {
        case RegionId::FungalDepths: return Biome::Fungal;
        case RegionId::CrystalCaverns: return Biome::Crystal;
        case RegionId::DrownedHalls: return Biome::Flooded;
        case RegionId::InfernalFoundry: return Biome::Infernal;
        case RegionId::AshenFortress: return Biome::Ruins;
        case RegionId::AbyssalTemple: return Biome::Abyssal;
        case RegionId::Shadowdeep: return rng.chance(50) ? Biome::Void : Biome::Abyssal;
        default: return Biome::Stone;
    }
}

void DungeonGenerator::generate(Dungeon& d, int depth) {
    d.depth = depth;
    d.regionId = static_cast<int>(regionForDepth(depth));
    d.biome = biomeForDepth(depth, rng_);
    clear(d);

    int styleRoll = rng_.range(0, 99);
    if (depth >= 15 && depth < 30 && styleRoll < 15) {
        generateCavern(d);
    } else if (depth >= 30 && styleRoll < 25) {
        generateCavern(d);
    } else if (styleRoll < 30 && depth > 2) {
        generateBsp(d);
    } else {
        generateRooms(d, depth);
    }

    connectRooms(d);
    placeDoors(d);
    placeFeatures(d, depth);

    if (!validateConnectivity(d)) {
        generateRooms(d, depth);
        connectRooms(d);
        placeDoors(d);
        placeFeatures(d, depth);
    }

    d.camera.viewW = kViewportW;
    d.camera.viewH = kViewportH;
}

void DungeonGenerator::clear(Dungeon& d) {
    d.clear();
}

void DungeonGenerator::generateRooms(Dungeon& d, int depth) {
    int scaledW = kMapW;
    int scaledH = kMapH;
    if (depth > 30) {
        scaledW = std::min(kMaxMapW, kMapW + (depth - 30) * 2);
        scaledH = std::min(kMaxMapH, kMapH + (depth - 30));
        d.mapW = scaledW;
        d.mapH = scaledH;
    }

    int baseRooms = 14 + depth / 2 + rng_.range(0, 6);
    if (depth > 30) baseRooms += (depth - 30) / 2;
    if (baseRooms > 40) baseRooms = 40;
    int target = baseRooms;

    for (int tries = 0; tries < 2000 && static_cast<int>(d.rooms.size()) < target; ++tries) {
        int w = rng_.range(6, 18);
        int h = rng_.range(5, 12);
        if (depth >= 5 && rng_.chance(30)) {
            w = rng_.range(10, 24);
            h = rng_.range(8, 14);
        }
        if (depth > 30 && rng_.chance(40)) {
            w = rng_.range(12, 30);
            h = rng_.range(10, 18);
        }
        int x = rng_.range(1, d.mapW - w - 2);
        int y = rng_.range(1, d.mapH - h - 2);
        Rect r{x, y, w, h};
        bool good = true;
        for (auto& q : d.rooms) {
            if (r.intersects(q, 1)) {
                good = false;
                break;
            }
        }
        if (good) d.rooms.push_back(r);
    }

    if (d.rooms.size() < 3) {
        d.rooms.clear();
        d.rooms.push_back({3, 3, 14, 8});
        d.rooms.push_back({d.mapW - 20, d.mapH - 12, 16, 8});
        d.rooms.push_back({d.mapW / 2 - 6, d.mapH / 2 - 4, 12, 8});
    }

    for (auto& r : d.rooms) {
        for (int y = r.y; y < r.y + r.h; ++y) {
            for (int x = r.x; x < r.x + r.w; ++x) {
                Vec2 p{x, y};
                if (d.inBounds(p)) d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Floor;
            }
        }
    }

    d.stairsUp = d.rooms.front().center();
    d.stairsDown = d.rooms.back().center();
    d.set(d.stairsUp, Tile::StairsUp);
    d.set(d.stairsDown, Tile::StairsDown);
}

void DungeonGenerator::generateBsp(Dungeon& d) {
    std::vector<Rect> leaves;
    std::vector<Rect> stack;
    stack.push_back({1, 1, d.mapW - 2, d.mapH - 2});

    while (!stack.empty()) {
        Rect cur = stack.back();
        stack.pop_back();

        if (cur.w < 14 || cur.h < 10 || (cur.w < 24 && cur.h < 18 && rng_.chance(40))) {
            leaves.push_back(cur);
            continue;
        }

        bool splitH = cur.w > cur.h ? false : true;
        if (cur.w > cur.h * 1.5) splitH = false;
        else if (cur.h > cur.w * 1.5) splitH = true;
        else splitH = rng_.flip();

        if (splitH) {
            int split = rng_.range(cur.h / 3, cur.h * 2 / 3);
            Rect a{cur.x, cur.y, cur.w, split};
            Rect b{cur.x, cur.y + split, cur.w, cur.h - split};
            stack.push_back(a);
            stack.push_back(b);
        } else {
            int split = rng_.range(cur.w / 3, cur.w * 2 / 3);
            Rect a{cur.x, cur.y, split, cur.h};
            Rect b{cur.x + split, cur.y, cur.w - split, cur.h};
            stack.push_back(a);
            stack.push_back(b);
        }
    }

    for (auto& leaf : leaves) {
        int w = rng_.range(6, std::max(6, leaf.w - 2));
        int h = rng_.range(5, std::max(5, leaf.h - 2));
        int x = leaf.x + rng_.range(0, std::max(0, leaf.w - w - 1));
        int y = leaf.y + rng_.range(0, std::max(0, leaf.h - h - 1));
        Rect r{x, y, w, h};
        d.rooms.push_back(r);
        for (int yy = r.y; yy < r.y + r.h; ++yy) {
            for (int xx = r.x; xx < r.x + r.w; ++xx) {
                Vec2 p{xx, yy};
                if (d.inBounds(p)) d.tiles[static_cast<size_t>(yy)][static_cast<size_t>(xx)] = Tile::Floor;
            }
        }
    }

    if (d.rooms.empty()) {
        generateRooms(d, 1);
        return;
    }

    d.stairsUp = d.rooms.front().center();
    d.stairsDown = d.rooms.back().center();
    d.set(d.stairsUp, Tile::StairsUp);
    d.set(d.stairsDown, Tile::StairsDown);
}

void DungeonGenerator::generateCavern(Dungeon& d) {
    for (int y = 1; y < d.mapH - 1; ++y) {
        for (int x = 1; x < d.mapW - 1; ++x) {
            d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = rng_.chance(46) ? Tile::Wall : Tile::Floor;
        }
    }

    for (int iter = 0; iter < 5; ++iter) {
        auto copy = d.tiles;
        for (int y = 1; y < d.mapH - 1; ++y) {
            for (int x = 1; x < d.mapW - 1; ++x) {
                int walls = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        Vec2 p{x + dx, y + dy};
                        if (!d.inBounds(p) || copy[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)] == Tile::Wall) walls++;
                    }
                }
                if (walls >= 5) copy[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Wall;
                else copy[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Floor;
            }
        }
        d.tiles = copy;
    }

    std::vector<std::vector<bool>> visited(d.mapH, std::vector<bool>(d.mapW, false));
    std::vector<Rect> regions;

    for (int y = 1; y < d.mapH - 1; ++y) {
        for (int x = 1; x < d.mapW - 1; ++x) {
            if (visited[static_cast<size_t>(y)][static_cast<size_t>(x)]) continue;
            if (d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] == Tile::Wall) continue;

            std::queue<Vec2> q;
            q.push({x, y});
            visited[static_cast<size_t>(y)][static_cast<size_t>(x)] = true;
            int minX = x, maxX = x, minY = y, maxY = y;
            int count = 0;

            while (!q.empty()) {
                Vec2 cur = q.front(); q.pop();
                count++;
                minX = std::min(minX, cur.x);
                maxX = std::max(maxX, cur.x);
                minY = std::min(minY, cur.y);
                maxY = std::max(maxY, cur.y);
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (std::abs(dx) + std::abs(dy) != 1) continue;
                        Vec2 n{cur.x + dx, cur.y + dy};
                        if (!d.inBounds(n)) continue;
                        if (visited[static_cast<size_t>(n.y)][static_cast<size_t>(n.x)]) continue;
                        if (d.tiles[static_cast<size_t>(n.y)][static_cast<size_t>(n.x)] == Tile::Wall) continue;
                        visited[static_cast<size_t>(n.y)][static_cast<size_t>(n.x)] = true;
                        q.push(n);
                    }
                }
            }

            if (count > 40) {
                regions.push_back({minX, minY, maxX - minX + 1, maxY - minY + 1});
            }
        }
    }

    if (regions.size() >= 2) {
        std::sort(regions.begin(), regions.end(), [](const Rect& a, const Rect& b){ return a.x < b.x; });
        d.rooms = regions;
        for (size_t i = 1; i < regions.size(); ++i) {
            connectRects(d, regions[i-1], regions[i]);
        }
    } else {
        generateRooms(d, 1);
        return;
    }

    d.stairsUp = d.rooms.front().center();
    d.stairsDown = d.rooms.back().center();
    d.set(d.stairsUp, Tile::StairsUp);
    d.set(d.stairsDown, Tile::StairsDown);
}

void DungeonGenerator::connectRooms(Dungeon& d) {
    if (d.rooms.size() < 2) return;
    for (size_t i = 1; i < d.rooms.size(); ++i) {
        connectRects(d, d.rooms[i-1], d.rooms[i]);
    }
    int extra = 6 + static_cast<int>(d.rooms.size()) / 4;
    for (int i = 0; i < extra; ++i) {
        int a = rng_.range(0, static_cast<int>(d.rooms.size()) - 1);
        int b = rng_.range(0, static_cast<int>(d.rooms.size()) - 1);
        if (a != b) connectRects(d, d.rooms[static_cast<size_t>(a)], d.rooms[static_cast<size_t>(b)]);
    }
}

void DungeonGenerator::placeDoors(Dungeon& d) {
    for (int y = 1; y < d.mapH - 1; ++y) {
        for (int x = 1; x < d.mapW - 1; ++x) {
            if (d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] != Tile::Corridor) continue;
            if (!rng_.chance(10)) continue;
            bool lr = d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x-1)] != Tile::Wall && d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x+1)] != Tile::Wall && d.tiles[static_cast<size_t>(y-1)][static_cast<size_t>(x)] == Tile::Wall && d.tiles[static_cast<size_t>(y+1)][static_cast<size_t>(x)] == Tile::Wall;
            bool ud = d.tiles[static_cast<size_t>(y-1)][static_cast<size_t>(x)] != Tile::Wall && d.tiles[static_cast<size_t>(y+1)][static_cast<size_t>(x)] != Tile::Wall && d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x-1)] == Tile::Wall && d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x+1)] == Tile::Wall;
            if (lr || ud) d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::DoorClosed;
        }
    }
}

void DungeonGenerator::placeFeatures(Dungeon& d, int depth) {
    int rubble = rng_.range(4, 10 + depth / 3);
    for (int i = 0; i < rubble; ++i) {
        if (d.rooms.empty()) break;
        auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
        Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
        if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Rubble);
    }

    if (rng_.chance(30 + depth)) {
        for (int i = 0; i < rng_.range(1, 4); ++i) {
            if (d.rooms.empty()) break;
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) {
                d.set(p, Tile::Water);
            }
        }
    }

    if (depth >= 8 && rng_.chance(30)) {
        for (int i = 0; i < rng_.range(1, 3); ++i) {
            if (d.rooms.empty()) break;
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Lava);
        }
    }

    if (rng_.chance(20)) {
        if (!d.rooms.empty()) {
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Fountain);
        }
    }

    if (rng_.chance(25)) {
        if (!d.rooms.empty()) {
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Chest);
        }
    }

    if (depth >= 10 && rng_.chance(20)) {
        if (!d.rooms.empty()) {
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Altar);
        }
    }

    if (depth >= 20 && rng_.chance(15)) {
        if (!d.rooms.empty()) {
            auto& r = d.rooms[static_cast<size_t>(rng_.range(0, static_cast<int>(d.rooms.size()) - 1))];
            Vec2 p{rng_.range(r.x, r.x + r.w - 1), rng_.range(r.y, r.y + r.h - 1)};
            if (p != d.stairsUp && p != d.stairsDown && d.at(p) == Tile::Floor) d.set(p, Tile::Trap);
        }
    }
}

void DungeonGenerator::carveH(Dungeon& d, int a, int b, int y) {
    if (a > b) std::swap(a, b);
    for (int x = a; x <= b; ++x) {
        if (x > 0 && x < d.mapW - 1 && y > 0 && y < d.mapH - 1) {
            if (d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] == Tile::Wall) d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Corridor;
        }
    }
}

void DungeonGenerator::carveV(Dungeon& d, int a, int b, int x) {
    if (a > b) std::swap(a, b);
    for (int y = a; y <= b; ++y) {
        if (x > 0 && x < d.mapW - 1 && y > 0 && y < d.mapH - 1) {
            if (d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] == Tile::Wall) d.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] = Tile::Corridor;
        }
    }
}

void DungeonGenerator::connectRects(Dungeon& d, const Rect& a, const Rect& b) {
    Vec2 p = a.center();
    Vec2 q = b.center();
    if (rng_.flip()) {
        carveH(d, p.x, q.x, p.y);
        carveV(d, p.y, q.y, q.x);
    } else {
        carveV(d, p.y, q.y, p.x);
        carveH(d, p.x, q.x, q.y);
    }
}

bool DungeonGenerator::validateConnectivity(const Dungeon& d) {
    if (!d.inBounds(d.stairsUp) || !d.inBounds(d.stairsDown)) return false;

    std::queue<Vec2> qq;
    std::vector<std::vector<bool>> vis(static_cast<size_t>(d.mapH), std::vector<bool>(static_cast<size_t>(d.mapW), false));
    qq.push(d.stairsUp);
    vis[static_cast<size_t>(d.stairsUp.y)][static_cast<size_t>(d.stairsUp.x)] = true;

    while (!qq.empty()) {
        Vec2 cur = qq.front(); qq.pop();
        if (cur == d.stairsDown) return true;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                if (std::abs(dx) + std::abs(dy) == 2) continue;
                Vec2 n{cur.x + dx, cur.y + dy};
                if (!d.inBounds(n)) continue;
                if (vis[static_cast<size_t>(n.y)][static_cast<size_t>(n.x)]) continue;
                if (d.blocksMove(n)) continue;
                vis[static_cast<size_t>(n.y)][static_cast<size_t>(n.x)] = true;
                qq.push(n);
            }
        }
    }
    return false;
}

}
