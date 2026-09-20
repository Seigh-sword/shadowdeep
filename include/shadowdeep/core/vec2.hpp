#pragma once
#include <algorithm>
#include <cstdlib>

namespace shadowdeep {

struct Vec2 {
    int x = 0;
    int y = 0;

    constexpr Vec2() = default;
    constexpr Vec2(int X, int Y) : x(X), y(Y) {}

    constexpr Vec2 operator+(const Vec2& o) const {
        return {x + o.x, y + o.y};
    }

    constexpr Vec2 operator-(const Vec2& o) const {
        return {x - o.x, y - o.y};
    }

    constexpr Vec2& operator+=(const Vec2& o) {
        x += o.x;
        y += o.y;
        return *this;
    }

    constexpr bool operator==(const Vec2& o) const {
        return x == o.x && y == o.y;
    }

    constexpr bool operator!=(const Vec2& o) const {
        return !(*this == o);
    }

    constexpr int manhattan(const Vec2& o) const {
        return std::abs(x - o.x) + std::abs(y - o.y);
    }

    constexpr int chebyshev(const Vec2& o) const {
        return std::max(std::abs(x - o.x), std::abs(y - o.y));
    }

    constexpr int lengthSq() const {
        return x * x + y * y;
    }
};

}
