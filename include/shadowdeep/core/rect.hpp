#pragma once
#include "vec2.hpp"

namespace shadowdeep {

struct Rect {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    constexpr Vec2 center() const {
        return {x + w / 2, y + h / 2};
    }

    constexpr Vec2 min() const {
        return {x, y};
    }

    constexpr Vec2 max() const {
        return {x + w, y + h};
    }

    constexpr bool contains(Vec2 p) const {
        return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
    }

    constexpr bool intersects(const Rect& o, int pad = 0) const {
        return !(x + w + pad <= o.x ||
                 o.x + o.w + pad <= x ||
                 y + h + pad <= o.y ||
                 o.y + o.h + pad <= y);
    }
};

}
