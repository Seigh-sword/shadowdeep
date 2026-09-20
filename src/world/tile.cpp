#include "shadowdeep/world/tile.hpp"

namespace shadowdeep {

char tileGlyph(Tile t) {
    switch (t) {
        case Tile::Wall: return '#';
        case Tile::Floor: return '.';
        case Tile::Corridor: return '.';
        case Tile::DoorClosed: return '+';
        case Tile::DoorOpen: return '/';
        case Tile::StairsDown: return '>';
        case Tile::StairsUp: return '<';
        case Tile::Rubble: return '*';
        case Tile::Water: return '~';
        case Tile::DeepWater: return '~';
        case Tile::Lava: return '~';
        case Tile::Chasm: return ' ';
        case Tile::Fungus: return 'f';
        case Tile::Web: return 'w';
        case Tile::Grass: return '"';
        case Tile::Ice: return '.';
        case Tile::Altar: return '_';
        case Tile::Fountain: return '{';
        case Tile::Chest: return '=';
        case Tile::Statue: return '&';
        default: return ' ';
    }
}

const char* tileName(Tile t) {
    switch (t) {
        case Tile::Wall: return "wall";
        case Tile::Floor: return "floor";
        case Tile::Corridor: return "corridor";
        case Tile::DoorClosed: return "closed door";
        case Tile::DoorOpen: return "open door";
        case Tile::StairsDown: return "stairs down";
        case Tile::StairsUp: return "stairs up";
        case Tile::Rubble: return "rubble";
        case Tile::Water: return "shallow water";
        case Tile::DeepWater: return "deep water";
        case Tile::Lava: return "lava";
        case Tile::Chasm: return "chasm";
        case Tile::Fungus: return "fungus";
        case Tile::Web: return "webs";
        case Tile::Grass: return "grass";
        case Tile::Ice: return "ice";
        case Tile::Altar: return "altar";
        case Tile::Fountain: return "fountain";
        case Tile::Chest: return "chest";
        case Tile::Statue: return "statue";
        default: return "unknown";
    }
}

}
