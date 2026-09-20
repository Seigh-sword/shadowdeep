#pragma once
#include <cstdint>
#include <string>

namespace shadowdeep {

enum class DamageType : uint8_t {
    Physical = 0,
    Fire = 1,
    Frost = 2,
    Lightning = 3,
    Poison = 4,
    Holy = 5,
    Shadow = 6,
    Arcane = 7,
    Count = 8
};

struct Damage {
    int amount = 0;
    DamageType type = DamageType::Physical;
    bool critical = false;
};

std::string damageTypeName(DamageType t);
std::string damageTypeShort(DamageType t);

}
