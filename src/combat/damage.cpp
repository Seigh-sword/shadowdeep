#include "shadowdeep/combat/damage.hpp"

namespace shadowdeep {

std::string damageTypeName(DamageType t) {
    switch (t) {
        case DamageType::Physical: return "Physical";
        case DamageType::Fire: return "Fire";
        case DamageType::Frost: return "Frost";
        case DamageType::Lightning: return "Lightning";
        case DamageType::Poison: return "Poison";
        case DamageType::Holy: return "Holy";
        case DamageType::Shadow: return "Shadow";
        case DamageType::Arcane: return "Arcane";
        default: return "Unknown";
    }
}

std::string damageTypeShort(DamageType t) {
    switch (t) {
        case DamageType::Physical: return "Phys";
        case DamageType::Fire: return "Fire";
        case DamageType::Frost: return "Frost";
        case DamageType::Lightning: return "Light";
        case DamageType::Poison: return "Pois";
        case DamageType::Holy: return "Holy";
        case DamageType::Shadow: return "Shad";
        case DamageType::Arcane: return "Arca";
        default: return "Unk";
    }
}

}
