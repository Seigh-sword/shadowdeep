#include "shadowdeep/entities/player.hpp"

namespace shadowdeep {

int Player::attackPower() const {
    int base = stats.baseAtk + stats.strength + equipment.totalAttack();
    for (auto& e : effects.all()) {
        if (e.id == EffectId::Strength) base += e.power;
        if (e.id == EffectId::Blessed) base += 2;
        if (e.id == EffectId::Cursed) base -= 2;
    }
    return base < 1 ? 1 : base;
}

int Player::defensePower() const {
    int base = stats.baseDef + equipment.totalArmor();
    for (auto& e : effects.all()) {
        if (e.id == EffectId::Shielded) base += e.power;
        if (e.id == EffectId::Blessed) base += 1;
    }
    return base < 0 ? 0 : base;
}

bool Player::gainXp(int amount) {
    stats.xp += amount;
    bool leveled = false;
    while (stats.xp >= stats.xpNext) {
        stats.xp -= stats.xpNext;
        stats.level++;
        stats.maxHp += 6;
        stats.hp = stats.maxHp;
        stats.baseAtk++;
        stats.xpNext = static_cast<int>(stats.xpNext * 1.65) + 10;
        leveled = true;
    }
    return leveled;
}

std::string Player::className() const {
    return className(pclass);
}

std::string Player::className(PlayerClass c) {
    switch (c) {
        case PlayerClass::Warrior: return "Warrior";
        case PlayerClass::Rogue: return "Rogue";
        case PlayerClass::Ranger: return "Ranger";
        case PlayerClass::Arcanist: return "Arcanist";
        case PlayerClass::Cleric: return "Cleric";
        case PlayerClass::Warden: return "Warden";
        default: return "Unknown";
    }
}

std::vector<PlayerClass> Player::allClasses() {
    return {PlayerClass::Warrior, PlayerClass::Rogue, PlayerClass::Ranger, PlayerClass::Arcanist, PlayerClass::Cleric, PlayerClass::Warden};
}

}
