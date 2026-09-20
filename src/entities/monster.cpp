#include "shadowdeep/entities/monster.hpp"

namespace shadowdeep {

const std::vector<MonsterTemplate>& allMonsterTemplates() {
    static std::vector<MonsterTemplate> templates = {
        {"monster.giant_rat", "giant rat", 'r', Color::Brown, MonsterFamily::Beast, 4, 2, 2, 0, 3, 1, 4, 100, false, false, false, DamageType::Physical, {}},
        {"monster.cave_bat", "cave bat", 'b', Color::Gray, MonsterFamily::Beast, 3, 2, 2, 0, 4, 1, 5, 150, true, false, true, DamageType::Physical, {}},
        {"monster.kobold", "kobold", 'k', Color::Yellow, MonsterFamily::Humanoid, 7, 3, 3, 0, 6, 1, 5, 100, false, true, false, DamageType::Physical, {}},
        {"monster.goblin", "goblin", 'g', Color::Green, MonsterFamily::Humanoid, 11, 4, 4, 1, 10, 2, 6, 100, false, true, false, DamageType::Physical, {}},
        {"monster.goblin_archer", "goblin archer", 'g', Color::BrightGreen, MonsterFamily::Humanoid, 9, 3, 5, 0, 12, 2, 7, 110, false, false, false, DamageType::Physical, {}},
        {"monster.goblin_shaman", "goblin shaman", 'g', Color::BrightMagenta, MonsterFamily::Humanoid, 10, 3, 4, 1, 15, 3, 7, 100, false, true, false, DamageType::Shadow, {EffectId::Cursed}},
        {"monster.giant_spider", "giant spider", 'x', Color::Magenta, MonsterFamily::Beast, 9, 3, 4, 0, 12, 2, 6, 110, false, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.venom_spider", "venom spider", 'x', Color::BrightMagenta, MonsterFamily::Beast, 14, 4, 6, 1, 20, 4, 8, 120, false, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.skeleton", "skeleton", 's', Color::BrightWhite, MonsterFamily::Undead, 16, 5, 5, 2, 18, 3, 7, 100, false, true, false, DamageType::Physical, {}},
        {"monster.skeleton_archer", "skeleton archer", 's', Color::White, MonsterFamily::Undead, 14, 4, 6, 1, 22, 3, 8, 100, false, false, false, DamageType::Physical, {}},
        {"monster.zombie", "zombie", 'z', Color::Green, MonsterFamily::Undead, 22, 8, 6, 1, 22, 3, 8, 70, false, true, false, DamageType::Physical, {}},
        {"monster.ghoul", "ghoul", 'z', Color::BrightRed, MonsterFamily::Undead, 28, 6, 8, 2, 35, 4, 9, 90, false, true, false, DamageType::Shadow, {EffectId::Poison}},
        {"monster.orc", "orc", 'o', Color::Green, MonsterFamily::Humanoid, 26, 8, 7, 3, 30, 4, 9, 100, false, true, false, DamageType::Physical, {}},
        {"monster.orc_warrior", "orc warrior", 'o', Color::BrightGreen, MonsterFamily::Humanoid, 34, 8, 9, 4, 45, 5, 10, 95, false, true, false, DamageType::Physical, {}},
        {"monster.orc_shaman", "orc shaman", 'o', Color::BrightMagenta, MonsterFamily::Humanoid, 30, 6, 8, 2, 50, 5, 11, 100, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.ogre", "ogre", 'O', Color::Brown, MonsterFamily::Humanoid, 40, 10, 10, 4, 48, 5, 9, 80, false, true, false, DamageType::Physical, {}},
        {"monster.wraith", "wraith", 'w', Color::BrightCyan, MonsterFamily::Undead, 30, 6, 11, 5, 60, 6, 10, 120, false, true, true, DamageType::Shadow, {EffectId::Fear}},
        {"monster.vampire", "vampire", 'V', Color::BrightRed, MonsterFamily::Undead, 46, 10, 12, 5, 85, 6, 10, 110, false, true, false, DamageType::Shadow, {EffectId::Bleeding}},
        {"monster.troll", "troll", 'T', Color::Green, MonsterFamily::Humanoid, 65, 15, 14, 7, 110, 7, 10, 80, false, true, false, DamageType::Physical, {EffectId::Regeneration}},
        {"monster.slime", "slime", 'j', Color::Green, MonsterFamily::Aberration, 20, 6, 6, 1, 25, 2, 9, 60, true, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.mimic", "mimic", '=', Color::Brown, MonsterFamily::Aberration, 35, 10, 10, 5, 70, 4, 12, 90, false, false, false, DamageType::Physical, {}},
        {"monster.cultist", "cultist", 'c', Color::Purple, MonsterFamily::Humanoid, 28, 6, 9, 2, 40, 5, 12, 100, false, true, false, DamageType::Shadow, {EffectId::Cursed}},
        {"monster.necromancer", "necromancer", 'n', Color::Purple, MonsterFamily::Humanoid, 40, 8, 12, 3, 90, 7, 15, 100, false, true, false, DamageType::Shadow, {EffectId::Fear}},
        {"monster.elemental_fire", "fire elemental", 'E', Color::BrightRed, MonsterFamily::Construct, 45, 10, 13, 4, 80, 8, 16, 100, false, false, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.elemental_frost", "frost elemental", 'E', Color::BrightCyan, MonsterFamily::Construct, 45, 10, 12, 5, 80, 8, 16, 100, false, false, false, DamageType::Frost, {EffectId::Chilled}},
        {"monster.golem", "golem", 'G', Color::Steel, MonsterFamily::Construct, 70, 15, 15, 10, 120, 9, 18, 60, false, true, false, DamageType::Physical, {}},
        {"monster.dark_knight", "dark knight", 'K', Color::BrightWhite, MonsterFamily::Humanoid, 80, 15, 18, 8, 150, 12, 20, 90, false, true, false, DamageType::Physical, {EffectId::Fear}},
        {"monster.lich", "lich", 'L', Color::BrightMagenta, MonsterFamily::Undead, 90, 20, 20, 8, 200, 15, 25, 100, false, true, true, DamageType::Shadow, {EffectId::Cursed, EffectId::Fear}},
        {"monster.hydra", "hydra", 'H', Color::Green, MonsterFamily::Beast, 120, 20, 18, 6, 250, 18, 27, 85, false, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.demon", "demon", 'd', Color::BrightRed, MonsterFamily::Demon, 80, 15, 17, 9, 160, 8, 15, 100, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.shadow_beast", "shadow beast", 'b', Color::Purple, MonsterFamily::Aberration, 70, 12, 16, 6, 140, 14, 22, 130, true, false, false, DamageType::Shadow, {EffectId::Blinded}},
        {"monster.dragon_young", "young dragon", 'D', Color::BrightYellow, MonsterFamily::Dragon, 100, 20, 20, 8, 300, 20, 28, 90, false, true, true, DamageType::Fire, {EffectId::Burning}},
        {"monster.ancient_dragon", "ancient dragon", 'D', Color::Gold, MonsterFamily::Dragon, 240, 40, 26, 13, 800, 30, 30, 90, false, true, true, DamageType::Fire, {EffectId::Burning, EffectId::Fear}},
    };
    return templates;
}

const MonsterTemplate* findMonsterTemplate(const std::string& stableId) {
    for (auto& t : allMonsterTemplates()) {
        if (t.stableId == stableId) return &t;
    }
    return nullptr;
}

}
