#pragma once
#include "../core/vec2.hpp"
#include "dungeon.hpp"

namespace shadowdeep {

void computeFovShadowcast(Dungeon& d, Vec2 origin, int radius);
void computeFovBresenham(Dungeon& d, Vec2 origin, int radius);

}
