#include "shadowdeep/entities/player.hpp"
#include <cassert>

void test_combat() {
    shadowdeep::Player p;
    int atk = p.attackPower();
    assert(atk >= 1);
    int def = p.defensePower();
    assert(def >= 0);
}
