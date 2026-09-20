#include "shadowdeep/npc/npc.hpp"

namespace shadowdeep {

const std::vector<Npc>& allNpcTemplates() {
    static std::vector<Npc> npcs = {
        {"npc.merchant_general", "General Merchant", 'M', Color::BrightYellow, {0,0}, NpcType::Merchant, false, true, "dialogue.merchant_general", "", 0, false, 1},
        {"npc.blacksmith", "Blacksmith", 'B', Color::Steel, {0,0}, NpcType::Blacksmith, false, true, "dialogue.blacksmith", "", 1, false, 2},
        {"npc.enchanter", "Enchanter", 'E', Color::BrightMagenta, {0,0}, NpcType::Enchanter, false, true, "dialogue.enchanter", "", 2, false, 3},
        {"npc.healer", "Healer", 'H', Color::BrightGreen, {0,0}, NpcType::Healer, false, true, "dialogue.healer", "", -1, false, 4},
        {"npc.prisoner", "Prisoner", 'p', Color::White, {0,0}, NpcType::Prisoner, false, true, "dialogue.prisoner", "quest.rescue_prisoner", -1, false, 5},
    };
    return npcs;
}

}
