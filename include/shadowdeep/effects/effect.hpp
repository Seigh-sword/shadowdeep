#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace shadowdeep {

enum class EffectId : uint16_t {
    None = 0,
    Poison = 1,
    Bleeding = 2,
    Burning = 3,
    Chilled = 4,
    Frozen = 5,
    Stunned = 6,
    Confused = 7,
    Blinded = 8,
    Rooted = 9,
    Haste = 10,
    Slow = 11,
    Regeneration = 12,
    Shielded = 13,
    Cursed = 14,
    Blessed = 15,
    Fear = 16,
    Strength = 17,
    Invisible = 18,
    Count = 19
};

struct StatusEffect {
    EffectId id = EffectId::None;
    int duration = 0;
    int power = 0;
    int stacks = 1;
    std::string source;

    bool active() const {
        return id != EffectId::None && duration != 0;
    }

    std::string stableId() const;
    std::string displayName() const;
};

class EffectManager {
public:
    void add(EffectId id, int duration, int power = 0, std::string source = "");
    void remove(EffectId id);
    bool has(EffectId id) const;
    int getPower(EffectId id) const;
    void tick();
    void clear();
    const std::vector<StatusEffect>& all() const {
        return effects_;
    }

private:
    std::vector<StatusEffect> effects_;
};

}
