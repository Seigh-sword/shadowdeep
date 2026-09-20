#include "shadowdeep/world/fov.hpp"

namespace shadowdeep {

void computeFovBresenham(Dungeon& d, Vec2 origin, int radius) {
    d.computeFov(origin, radius);
}

void computeFovShadowcast(Dungeon& d, Vec2 origin, int radius) {
    d.computeFov(origin, radius);
}

}
