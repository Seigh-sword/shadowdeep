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

        {"monster.giant_ant", "giant ant", 'a', Color::Brown, MonsterFamily::Beast, 6, 3, 3, 1, 8, 1, 6, 110, false, true, false, DamageType::Physical, {}},
        {"monster.wolf", "wolf", 'w', Color::Gray, MonsterFamily::Beast, 12, 4, 5, 1, 15, 2, 8, 130, false, true, false, DamageType::Physical, {}},
        {"monster.dire_wolf", "dire wolf", 'w', Color::BrightWhite, MonsterFamily::Beast, 22, 6, 7, 2, 30, 3, 10, 140, false, true, false, DamageType::Physical, {}},
        {"monster.bear", "bear", 'b', Color::Brown, MonsterFamily::Beast, 35, 10, 8, 3, 50, 4, 12, 90, false, true, false, DamageType::Physical, {}},
        {"monster.giant_scorpion", "giant scorpion", 's', Color::Yellow, MonsterFamily::Beast, 28, 7, 8, 3, 40, 4, 11, 100, false, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.basilisk", "basilisk", 'b', Color::Green, MonsterFamily::Beast, 50, 12, 10, 5, 90, 6, 14, 80, false, true, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.cockatrice", "cockatrice", 'c', Color::BrightYellow, MonsterFamily::Beast, 30, 8, 9, 2, 60, 5, 13, 110, false, true, false, DamageType::Poison, {EffectId::Blinded}},
        {"monster.harpy", "harpy", 'h', Color::Gray, MonsterFamily::Beast, 25, 6, 8, 2, 45, 4, 12, 120, false, false, true, DamageType::Physical, {}},
        {"monster.manticore", "manticore", 'M', Color::Brown, MonsterFamily::Beast, 70, 14, 14, 6, 130, 8, 18, 100, false, true, false, DamageType::Physical, {EffectId::Bleeding}},
        {"monster.chimera", "chimera", 'C', Color::BrightRed, MonsterFamily::Beast, 90, 18, 16, 7, 180, 10, 22, 90, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.wyvern", "wyvern", 'W', Color::Green, MonsterFamily::Dragon, 80, 16, 15, 6, 160, 9, 20, 110, false, true, true, DamageType::Poison, {EffectId::Poison}},
        {"monster.ice_drake", "ice drake", 'D', Color::BrightCyan, MonsterFamily::Dragon, 110, 22, 18, 9, 280, 14, 24, 90, false, true, true, DamageType::Frost, {EffectId::Chilled}},
        {"monster.shadow_drake", "shadow drake", 'D', Color::Purple, MonsterFamily::Dragon, 130, 24, 20, 10, 320, 16, 26, 100, false, true, true, DamageType::Shadow, {EffectId::Fear}},
        {"monster.bandit", "bandit", 'h', Color::Yellow, MonsterFamily::Humanoid, 14, 4, 5, 1, 20, 2, 9, 100, false, true, false, DamageType::Physical, {}},
        {"monster.bandit_leader", "bandit leader", 'h', Color::BrightYellow, MonsterFamily::Humanoid, 32, 8, 8, 3, 55, 5, 12, 100, false, true, false, DamageType::Physical, {}},
        {"monster.assassin", "assassin", 'a', Color::Gray, MonsterFamily::Humanoid, 35, 8, 12, 2, 70, 6, 14, 130, false, true, false, DamageType::Physical, {EffectId::Bleeding}},
        {"monster.berserker", "berserker", 'b', Color::BrightRed, MonsterFamily::Humanoid, 45, 12, 10, 3, 80, 6, 15, 110, false, true, false, DamageType::Physical, {EffectId::Regeneration}},
        {"monster.warlock", "warlock", 'w', Color::Purple, MonsterFamily::Humanoid, 40, 8, 11, 3, 85, 7, 16, 100, false, true, false, DamageType::Shadow, {EffectId::Cursed}},
        {"monster.pyromancer", "pyromancer", 'p', Color::BrightRed, MonsterFamily::Humanoid, 38, 7, 12, 2, 75, 6, 16, 100, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.cryomancer", "cryomancer", 'p', Color::BrightCyan, MonsterFamily::Humanoid, 38, 7, 11, 3, 75, 6, 16, 100, false, true, false, DamageType::Frost, {EffectId::Chilled}},
        {"monster.druid", "druid", 'd', Color::Green, MonsterFamily::Humanoid, 36, 8, 10, 3, 70, 5, 15, 100, false, true, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.spectre", "spectre", 's', Color::BrightCyan, MonsterFamily::Undead, 32, 7, 10, 4, 65, 5, 14, 120, false, true, true, DamageType::Shadow, {EffectId::Fear}},
        {"monster.banshee", "banshee", 'B', Color::BrightWhite, MonsterFamily::Undead, 42, 9, 11, 3, 80, 6, 16, 110, false, true, false, DamageType::Shadow, {EffectId::Fear}},
        {"monster.revenant", "revenant", 'R', Color::Gray, MonsterFamily::Undead, 55, 12, 12, 5, 100, 7, 18, 90, false, true, false, DamageType::Physical, {EffectId::Bleeding}},
        {"monster.death_knight", "death knight", 'K', Color::BrightRed, MonsterFamily::Undead, 75, 16, 16, 8, 170, 10, 22, 90, false, true, false, DamageType::Shadow, {EffectId::Cursed}},
        {"monster.bone_golem", "bone golem", 'G', Color::White, MonsterFamily::Construct, 65, 14, 13, 9, 130, 8, 19, 60, false, true, false, DamageType::Physical, {}},
        {"monster.flesh_golem", "flesh golem", 'G', Color::BrightRed, MonsterFamily::Construct, 75, 16, 14, 7, 150, 9, 20, 60, false, true, false, DamageType::Physical, {EffectId::Regeneration}},
        {"monster.iron_golem", "iron golem", 'G', Color::Steel, MonsterFamily::Construct, 95, 20, 16, 12, 200, 12, 24, 50, false, true, false, DamageType::Physical, {}},
        {"monster.imp", "imp", 'i', Color::BrightRed, MonsterFamily::Demon, 18, 5, 7, 1, 30, 3, 10, 130, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.hellhound", "hellhound", 'h', Color::BrightRed, MonsterFamily::Demon, 38, 9, 9, 3, 70, 5, 14, 120, false, true, false, DamageType::Fire, {EffectId::Burning}},
        {"monster.succubus", "succubus", 's', Color::BrightMagenta, MonsterFamily::Demon, 45, 9, 12, 3, 90, 6, 16, 110, false, true, false, DamageType::Shadow, {EffectId::Cursed}},
        {"monster.balor", "balor", 'B', Color::BrightRed, MonsterFamily::Demon, 110, 22, 19, 10, 250, 14, 26, 90, false, true, true, DamageType::Fire, {EffectId::Burning, EffectId::Fear}},
        {"monster.shadow_lurker", "shadow lurker", 'l', Color::Purple, MonsterFamily::Aberration, 30, 7, 10, 4, 60, 5, 15, 130, true, false, false, DamageType::Shadow, {EffectId::Blinded}},
        {"monster.mind_flayer", "mind flayer", 'm', Color::Purple, MonsterFamily::Aberration, 60, 12, 14, 4, 120, 8, 20, 100, false, true, false, DamageType::Shadow, {EffectId::Fear, EffectId::Cursed}},
        {"monster.beholder", "beholder", 'B', Color::BrightMagenta, MonsterFamily::Aberration, 85, 16, 16, 6, 180, 10, 24, 80, true, false, false, DamageType::Shadow, {EffectId::Blinded, EffectId::Fear}},
        {"monster.ooze", "ooze", 'o', Color::Green, MonsterFamily::Aberration, 26, 8, 7, 2, 35, 3, 12, 60, true, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.carrion_crawler", "carrion crawler", 'c', Color::Brown, MonsterFamily::Aberration, 40, 9, 9, 3, 65, 5, 14, 70, false, false, false, DamageType::Poison, {EffectId::Poison}},
        {"monster.giant_worm", "giant worm", 'w', Color::Brown, MonsterFamily::Beast, 55, 14, 11, 4, 80, 6, 16, 70, false, false, false, DamageType::Physical, {EffectId::Bleeding}},
        {"monster.earth_elemental", "earth elemental", 'E', Color::Brown, MonsterFamily::Construct, 60, 14, 12, 10, 120, 7, 18, 60, false, true, false, DamageType::Physical, {}},
        {"monster.air_elemental", "air elemental", 'E', Color::White, MonsterFamily::Construct, 45, 9, 14, 3, 90, 7, 18, 130, false, false, true, DamageType::Physical, {}},
        {"monster.water_elemental", "water elemental", 'E', Color::BrightCyan, MonsterFamily::Construct, 50, 12, 12, 5, 100, 7, 18, 80, false, false, false, DamageType::Frost, {EffectId::Chilled}},

        {"monster.goblin_king", "goblin king", 'K', Color::Gold, MonsterFamily::Humanoid, 70, 14, 14, 6, 200, 10, 20, 90, false, true, false, DamageType::Physical, {EffectId::Fear}},
        {"monster.orc_warlord", "orc warlord", 'W', Color::BrightGreen, MonsterFamily::Humanoid, 90, 18, 16, 8, 250, 12, 22, 90, false, true, false, DamageType::Physical, {EffectId::Regeneration}},
        {"monster.lich_king", "lich king", 'L', Color::Gold, MonsterFamily::Undead, 130, 28, 22, 12, 400, 20, 28, 100, false, true, true, DamageType::Shadow, {EffectId::Cursed, EffectId::Fear, EffectId::Chilled}},
        {"monster.demon_lord", "demon lord", 'D', Color::BrightRed, MonsterFamily::Demon, 150, 30, 24, 14, 500, 22, 30, 100, false, true, true, DamageType::Fire, {EffectId::Burning, EffectId::Fear}},
        {"monster.shadow_lord", "shadow lord", 'S', Color::Purple, MonsterFamily::Aberration, 160, 32, 26, 15, 600, 25, 32, 110, true, true, true, DamageType::Shadow, {EffectId::Blinded, EffectId::Fear, EffectId::Cursed}},
        {"monster.dragon_elder", "elder dragon", 'D', Color::Gold, MonsterFamily::Dragon, 200, 35, 28, 16, 700, 28, 35, 90, false, true, true, DamageType::Fire, {EffectId::Burning, EffectId::Fear}},
        {"monster.void_horror", "void horror", 'V', Color::Purple, MonsterFamily::Aberration, 180, 30, 26, 12, 650, 26, 40, 100, true, true, true, DamageType::Shadow, {EffectId::Fear, EffectId::Blinded}},
        {"monster.titan", "titan", 'T', Color::BrightWhite, MonsterFamily::Construct, 220, 40, 30, 20, 800, 30, 45, 60, false, true, false, DamageType::Physical, {EffectId::Stunned}},
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
