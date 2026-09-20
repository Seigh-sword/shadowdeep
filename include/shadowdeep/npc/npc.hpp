#pragma once
#include <string>
#include <vector>
#include "../core/vec2.hpp"
#include "../terminal/color.hpp"

namespace shadowdeep {

enum class NpcType : uint8_t {
    Merchant = 0,
    Blacksmith = 1,
    Enchanter = 2,
    Healer = 3,
    Scholar = 4,
    Guard = 5,
    Prisoner = 6,
    Wanderer = 7
};

struct Npc {
    std::string stableId;
    std::string name;
    char glyph = '@';
    Color color = Color::BrightWhite;
    Vec2 pos{0,0};
    NpcType type = NpcType::Wanderer;
    bool rescued = false;
    bool alive = true;
    std::string dialogueId;
    std::string questId;
    int shopId = -1;
    bool isHostile = false;
    int id = 0;
};

const std::vector<Npc>& allNpcTemplates();

}
