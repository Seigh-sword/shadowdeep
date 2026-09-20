#pragma once
#include <chrono>
#include <cstdint>
#include <random>

namespace shadowdeep {

class Rng {
public:
    Rng() {
        uint64_t now = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );
        seed_ = static_cast<uint32_t>(now ^ (now >> 32));
        engine_.seed(seed_);
    }

    explicit Rng(uint32_t seed) : seed_(seed) {
        engine_.seed(seed_);
    }

    void reseed(uint32_t seed) {
        seed_ = seed;
        engine_.seed(seed_);
    }

    uint32_t seedValue() const {
        return seed_;
    }

    uint32_t nextU32() {
        return engine_();
    }

    int range(int lo, int hi) {
        if (hi < lo) std::swap(lo, hi);
        std::uniform_int_distribution<int> d(lo, hi);
        return d(engine_);
    }

    float rangeFloat(float lo, float hi) {
        if (hi < lo) std::swap(lo, hi);
        std::uniform_real_distribution<float> d(lo, hi);
        return d(engine_);
    }

    bool chance(int percent) {
        return range(1, 100) <= percent;
    }

    bool chanceFloat(float p) {
        return rangeFloat(0.0f, 1.0f) < p;
    }

    bool flip() {
        return chance(50);
    }

    std::mt19937& engine() {
        return engine_;
    }

    uint64_t stateForSave() const {
        return static_cast<uint64_t>(seed_) ^ 0x9E3779B97F4A7C15ULL;
    }

private:
    std::mt19937 engine_;
    uint32_t seed_ = 0;
};

}
