#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../core/vec2.hpp"
#include "../terminal/color.hpp"
#include "../effects/effect.hpp"
#include "../combat/damage.hpp"

namespace shadowdeep {

enum class MonsterFamily : uint8_t {
    Beast = 0,
    Humanoid = 1,
    Undead = 2,
    Demon = 3,
    Construct = 4,
    Dragon = 5,
    Aberration = 6
};

enum class AiState : uint8_t {
    Idle = 0,
    Wandering = 1,
    Suspicious = 2,
    Alert = 3,
    Hunting = 4,
    Fleeing = 5,
    Guarding = 6
};

struct MonsterTemplate {
    std::string stableId;
    std::string name;
    char glyph;
    std::string unicodeGlyph;
    Color color;
    MonsterFamily family;
    int baseHp;
    int hpExtra;
    int baseAtk;
    int baseDef;
    int xp;
    int minDepth;
    int maxDepth;
    int speed;
    bool erratic;
    bool canOpenDoors;
    bool canFly;
    DamageType damageType;
    std::vector<EffectId> abilities;
};

struct Monster {
    std::string stableId;
    std::string name;
    char glyph = '?';
    std::string unicodeGlyph;
    Color color = Color::White;
    Vec2 pos{0, 0};
    int hp = 1;
    int maxHp = 1;
    int atk = 1;
    int def = 0;
    int xp = 1;
    int speed = 100;
    int energy = 0;
    bool alive = true;
    AiState aiState = AiState::Idle;
    bool aware = false;
    bool erratic = false;
    bool boss = false;
    bool canOpenDoors = true;
    bool canFly = false;
    MonsterFamily family = MonsterFamily::Beast;
    DamageType damageType = DamageType::Physical;
    EffectManager effects;
    Vec2 lastSeenPlayer{-1, -1};
    Vec2 guardPos{-1, -1};
    int id = 0;

    bool isHostile() const {
        return alive;
    }

    std::string displayGlyph() const {
        if (!unicodeGlyph.empty()) return unicodeGlyph;
        return std::string(1, glyph);
    }
};

const std::vector<MonsterTemplate>& allMonsterTemplates();
const MonsterTemplate* findMonsterTemplate(const std::string& stableId);

}
