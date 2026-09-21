#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../core/vec2.hpp"
#include "../items/inventory.hpp"
#include "../effects/effect.hpp"

namespace shadowdeep {

enum class PlayerClass : uint8_t {
    Warrior = 0,
    Rogue = 1,
    Ranger = 2,
    Arcanist = 3,
    Cleric = 4,
    Warden = 5,
    Count = 6
};

enum class HungerState : uint8_t {
    Starving = 0,
    Hungry = 1,
    Normal = 2,
    Sated = 3,
    Full = 4,
    Engorged = 5
};

struct PlayerStats {
    int level = 1;
    int xp = 0;
    int xpNext = 15;
    int maxHp = 30;
    int hp = 30;
    int baseAtk = 4;
    int baseDef = 0;
    int strength = 0;
    int dexterity = 0;
    int intelligence = 0;
    int gold = 0;
    int rubies = 0;
    long long turns = 0;
    long long playTimeMs = 0;

    int hunger = 1000;
    int maxHunger = 2000;
    int saturation = 500;
    int maxSaturation = 2000;
    int hungerTick = 0;

    HungerState hungerState() const;
    std::string hungerName() const;
    bool isStarving() const;
    bool isHungry() const;
};

struct Player {
    std::string name = "Adventurer";
    PlayerClass pclass = PlayerClass::Warrior;
    Vec2 pos{0, 0};
    PlayerStats stats;
    Inventory inventory;
    Equipment equipment;
    EffectManager effects;
    bool hasAmulet = false;
    int hasteTurns = 0;
    std::string entryId;

    int attackPower() const;
    int defensePower() const;
    bool gainXp(int amount);
    bool isAlive() const {
        return stats.hp > 0;
    }
    void tickHunger();
    void eatFood(int nutrition, int sat);
    void addSaturation(int amount);
    bool canRegenFromSaturation() const;

    std::string className() const;
    static std::string className(PlayerClass c);
    static std::vector<PlayerClass> allClasses();
};

}
