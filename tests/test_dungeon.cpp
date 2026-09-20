#include "shadowdeep/world/dungeon.hpp"
#include "shadowdeep/world/generator.hpp"
#include "shadowdeep/core/rng.hpp"
#include <cassert>

void test_dungeon_connectivity() {
    shadowdeep::Rng rng(42);
    shadowdeep::Dungeon d;
    shadowdeep::DungeonGenerator gen(rng);
    gen.generate(d, 1);
    assert(d.inBounds(d.stairsUp));
    assert(d.inBounds(d.stairsDown));
    assert(d.walkable(d.stairsUp));
    assert(d.walkable(d.stairsDown));
}
