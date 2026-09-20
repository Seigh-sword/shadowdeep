#pragma once
#include "../core/rng.hpp"
#include "dungeon.hpp"

namespace shadowdeep {

class DungeonGenerator {
public:
    explicit DungeonGenerator(Rng& rng) : rng_(rng) {}

    void generate(Dungeon& d, int depth);

private:
    Rng& rng_;

    void clear(Dungeon& d);
    void generateRooms(Dungeon& d, int depth);
    void generateBsp(Dungeon& d);
    void generateCavern(Dungeon& d);
    void connectRooms(Dungeon& d);
    void placeDoors(Dungeon& d);
    void placeFeatures(Dungeon& d, int depth);
    void carveH(Dungeon& d, int a, int b, int y);
    void carveV(Dungeon& d, int a, int b, int x);
    void connectRects(Dungeon& d, const Rect& a, const Rect& b);
    bool validateConnectivity(const Dungeon& d);
};

}
