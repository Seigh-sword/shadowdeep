#pragma once
#include <cstdint>

namespace shadowdeep {

enum class Tile : uint8_t {
    Wall = 0,
    Floor = 1,
    Corridor = 2,
    DoorClosed = 3,
    DoorOpen = 4,
    StairsDown = 5,
    StairsUp = 6,
    Rubble = 7,
    Water = 8,
    DeepWater = 9,
    Lava = 10,
    Chasm = 11,
    Fungus = 12,
    Web = 13,
    Grass = 14,
    Ice = 15,
    Altar = 16,
    Fountain = 17,
    Chest = 18,
    Statue = 19,
    Trap = 20
};

inline bool tileBlocksMove(Tile t) {
    return t == Tile::Wall || t == Tile::DeepWater || t == Tile::Chasm || t == Tile::Statue;
}

inline bool tileBlocksSight(Tile t) {
    return t == Tile::Wall || t == Tile::DoorClosed;
}

inline bool tileWalkable(Tile t) {
    return !tileBlocksMove(t);
}

inline bool tileIsDoor(Tile t) {
    return t == Tile::DoorClosed || t == Tile::DoorOpen;
}

inline bool tileIsStairs(Tile t) {
    return t == Tile::StairsDown || t == Tile::StairsUp;
}

char tileGlyph(Tile t);
const char* tileName(Tile t);

}
