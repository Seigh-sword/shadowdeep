#include "shadowdeep/entities/player.hpp"

namespace shadowdeep {

HungerState PlayerStats::hungerState() const {
    if (hunger <= 0) return HungerState::Starving;
    if (hunger < 200) return HungerState::Hungry;
    if (hunger < 800) return HungerState::Normal;
    if (hunger < 1300) return HungerState::Sated;
    if (hunger < 1800) return HungerState::Full;
    return HungerState::Engorged;
}

std::string PlayerStats::hungerName() const {
    switch (hungerState()) {
        case HungerState::Starving: return "Starving";
        case HungerState::Hungry: return "Hungry";
        case HungerState::Normal: return "Normal";
        case HungerState::Sated: return "Sated";
        case HungerState::Full: return "Full";
        case HungerState::Engorged: return "Engorged";
        default: return "Normal";
    }
}

bool PlayerStats::isStarving() const {
    return hunger <= 0;
}

bool PlayerStats::isHungry() const {
    return hunger < 300;
}

int Player::attackPower() const {
    int base = stats.baseAtk + stats.strength + equipment.totalAttack();
    for (auto& e : effects.all()) {
        if (e.id == EffectId::Strength) base += e.power;
        if (e.id == EffectId::Blessed) base += 2;
        if (e.id == EffectId::Cursed) base -= 2;
    }
    if (stats.isStarving()) base = base * 3 / 4;
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

void Player::tickHunger() {
    stats.hungerTick++;
    if (stats.hungerTick < 10) return;
    stats.hungerTick = 0;

    if (stats.saturation > 0) {
        stats.saturation--;
        if (stats.saturation > 100) {
            if (stats.hp < stats.maxHp) {
                stats.hp++;
            }
        }
    } else {
        if (stats.hunger > 0) stats.hunger--;
    }

    if (stats.isStarving()) {
        if (stats.hungerTick % 2 == 0) {
            if (stats.hp > 1) stats.hp--;
        }
    }
}

void Player::eatFood(int nutrition, int sat) {
    stats.hunger += nutrition;
    if (stats.hunger > stats.maxHunger) stats.hunger = stats.maxHunger;
    stats.saturation += sat;
    if (stats.saturation > stats.maxSaturation) stats.saturation = stats.maxSaturation;
}

void Player::addSaturation(int amount) {
    stats.saturation += amount;
    if (stats.saturation > stats.maxSaturation) stats.saturation = stats.maxSaturation;
    if (stats.saturation < 0) stats.saturation = 0;
}

bool Player::canRegenFromSaturation() const {
    return stats.saturation > 100 && stats.hp < stats.maxHp;
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
