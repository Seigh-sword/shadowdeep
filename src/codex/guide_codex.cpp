#include "shadowdeep/codex/guide_codex.hpp"

namespace shadowdeep {

GuideCodex::GuideCodex() {
    initializeDefaults();
}

void GuideCodex::initializeDefaults() {
    guides_ = {
        {"guide.weapons_basic", {"guide.weapons_basic", "Basic Weapons", "Daggers, short swords and maces are common but reliable. Upgrade as soon as possible.", GuideCategory::Weapons, false, 0, 1, "dagger", ""}},
        {"guide.weapons_advanced", {"guide.weapons_advanced", "Advanced Arms", "Greatswords, battle axes and halberds deal massive damage but require strength. Enchantments add elemental power.", GuideCategory::Weapons, false, 0, 2, "longsword", ""}},
        {"guide.weapons_ranged", {"guide.weapons_ranged", "Ranged Combat", "Bows and crossbows let you strike from distance. Ammo is unlimited but damage is lower than melee.", GuideCategory::Weapons, false, 0, 2, "longbow", ""}},
        {"guide.weapons_legendary", {"guide.weapons_legendary", "Legendary Blades", "Demon blades, angel blades and the Shadowdeep Edge are mythic. They drop only from bosses beyond depth 20.", GuideCategory::Weapons, false, 0, 3, "demon blade", ""}},
        {"guide.armor_basic", {"guide.armor_basic", "Basic Protection", "Leather and chain protect lightly. Always wear something - even a cloth cap helps.", GuideCategory::Armor, false, 0, 1, "leather armour", ""}},
        {"guide.armor_heavy", {"guide.armor_heavy", "Heavy Plate", "Plate, mithril chain and adamantine plate offer huge defense but slow you slightly. Tower shields block most attacks.", GuideCategory::Armor, false, 0, 2, "plate armour", ""}},
        {"guide.armor_magical", {"guide.armor_magical", "Enchanted Attire", "Elven cloaks, robes of the archmagi and shadow plate grant resistances and magic bonuses.", GuideCategory::Armor, false, 0, 3, "elven cloak", ""}},
        {"guide.potions", {"guide.potions", "Potion Lore", "Red = healing, yellow = strength, cyan = haste/mana, green = antidote. Identify before drinking unknown brews.", GuideCategory::Potions, false, 0, 1, "potion of healing", ""}},
        {"guide.potions_rare", {"guide.potions_rare", "Rare Elixirs", "Rejuvenation fully restores, invisibility hides you from monsters, fire/frost/shadow resist potions grant temporary immunity.", GuideCategory::Potions, false, 0, 2, "potion of rejuvenation", ""}},
        {"guide.scrolls", {"guide.scrolls", "Scroll Mastery", "Lightning and fireball deal area damage. Teleport escapes danger. Mapping reveals the level. Identify reveals items.", GuideCategory::Scrolls, false, 0, 1, "scroll of lightning", ""}},
        {"guide.scrolls_rare", {"guide.scrolls_rare", "Forbidden Scrolls", "Enchantment upgrades weapons, Banishment removes enemies, Summoning calls allies - but at a cost.", GuideCategory::Scrolls, false, 0, 2, "scroll of enchantment", ""}},
        {"guide.food", {"guide.food", "Survival", "Hunger drains over time. Saturation from food heals you slowly. Starving deals damage. Always carry rations.", GuideCategory::Secrets, false, 0, 1, "food ration", ""}},
        {"guide.monsters_goblins", {"guide.monsters_goblins", "Goblin Kin", "Goblins are weak alone but deadly in groups. Shamans cast curses. Kings lead warrens.", GuideCategory::Monsters, false, 0, 1, "goblin", ""}},
        {"guide.monsters_undead", {"guide.monsters_undead", "Undead Hordes", "Skeletons and zombies fill crypts. Wraiths fear you, vampires bleed you, liches curse. Use radiant weapons.", GuideCategory::Monsters, false, 0, 2, "skeleton", ""}},
        {"guide.monsters_demons", {"guide.monsters_demons", "Demonic Incursion", "Imps, hellhounds and balors burn. Demon lords dominate deep levels. Fire resistance is essential beyond depth 16.", GuideCategory::Monsters, false, 0, 3, "demon", ""}},
        {"guide.monsters_dragons", {"guide.monsters_dragons", "Dragon Kin", "Young dragons breathe fire. Elder dragons are apex predators. Shadow drakes dwell in void tears.", GuideCategory::Monsters, false, 0, 3, "young dragon", ""}},
        {"guide.monsters_aberrations", {"guide.monsters_aberrations", "Aberrations", "Mind flayers, beholders and void horrors warp reality. Shadow resistance and high will save help.", GuideCategory::Monsters, false, 0, 3, "mind flayer", ""}},
        {"guide.biomes", {"guide.biomes", "Biome Guide", "Each region has a biome: Stone, Fungal, Crystal, Flooded, Infernal, Ruins, Abyssal, Void. Biomes affect monsters and loot.", GuideCategory::Biomes, false, 0, 2, "", ""}},
        {"guide.infinite_depth", {"guide.infinite_depth", "Infinite Depths", "Beyond depth 30, enemies scale infinitely stronger. Maps grow larger. Void tears appear. Only the bravest survive past 100.", GuideCategory::Biomes, false, 0, 3, "", ""}},
        {"guide.secrets", {"guide.secrets", "Secret Knowledge", "Search walls for hidden doors. Fountains may bless or curse. Altars grant boons. Chests may be mimics.", GuideCategory::Secrets, false, 0, 2, "", ""}},
        {"guide.amulet", {"guide.amulet", "The Amulet of Shadowdeep", "Legend says the Amulet lies at depth 30, guarded by the Shadow Lord. Retrieve it and escape to win. But deeper secrets await beyond.", GuideCategory::Lore, false, 0, 5, "Amulet of Shadowdeep", ""}},
    };

    lores_ = {
        {"lore.origin", {"lore.origin", "Origin of Shadowdeep", "In ages past, a meteor of pure shadow struck the mountain, creating a wound that never healed. The dungeon grew around it, hungering for light.", 1, false, ""}},
        {"lore.king", {"lore.king", "The Fallen King", "King Aldric built his fortress atop the wound to seal it. His knights became dark knights, his mages became liches. None escaped.", 5, false, ""}},
        {"lore.goblin_king", {"lore.goblin_king", "Goblin King Gruk", "Gruk the Many-Eyed united the warrens. He wears a crown of bone and commands shamans. Defeating him yields a guide fragment.", 8, true, "goblin king"}},
        {"lore.lich_king", {"lore.lich_king", "Lich King Moros", "Moros was once high priest of light. He sought to cure death and found only undeath. His phylactery is hidden in crystal caverns.", 20, true, "lich king"}},
        {"lore.demon_lord", {"lore.demon_lord", "Demon Lord Xaroth", "Xaroth clawed through from the infernal foundry. His forges produce cursed weapons. The infernal heat is his breath.", 25, true, "demon lord"}},
        {"lore.shadow_lord", {"lore.shadow_lord", "Shadow Lord", "The entity at depth 30 is not a monster but the wound itself given form. It whispers promises of power. Its defeat is only the beginning.", 30, true, "shadow lord"}},
        {"lore.void", {"lore.void", "Beyond Shadowdeep", "Those who descend past 30 report whispers of a deeper void - a place where reality frays, where titans sleep and horrors older than gods stir.", 35, false, ""}},
        {"lore.titan", {"lore.titan", "The Titan Slumber", "At depth 80, a titan sleeps encased in void crystal. Legends say waking it ends the world, but its heart grants mythic power.", 80, true, "titan"}},
    };
}

void GuideCodex::addFragment(const std::string& fragmentId, const std::string& sourceItem) {
    fragmentCounts_[fragmentId]++;
    totalFragmentsCollected_++;

    std::string lower = fragmentId;
    for (auto& c : lower) c = static_cast<char>(tolower(c));

    if (lower.find("weapon") != std::string::npos || lower.find("dagger") != std::string::npos || lower.find("sword") != std::string::npos || lower.find("axe") != std::string::npos) {
        if (fragmentCounts_[fragmentId] >= 1) unlockGuide("guide.weapons_basic");
        if (fragmentCounts_[fragmentId] >= 3) unlockGuide("guide.weapons_advanced");
    }
    if (lower.find("bow") != std::string::npos || lower.find("ranged") != std::string::npos) {
        unlockGuide("guide.weapons_ranged");
    }
    if (lower.find("legendary") != std::string::npos || lower.find("mythic") != std::string::npos) {
        unlockGuide("guide.weapons_legendary");
    }
    if (lower.find("armor") != std::string::npos || lower.find("plate") != std::string::npos || lower.find("leather") != std::string::npos) {
        unlockGuide("guide.armor_basic");
        if (fragmentCounts_[fragmentId] >= 2) unlockGuide("guide.armor_heavy");
    }
    if (lower.find("potion") != std::string::npos || lower.find("heal") != std::string::npos) {
        unlockGuide("guide.potions");
    }
    if (lower.find("scroll") != std::string::npos) {
        unlockGuide("guide.scrolls");
    }
    if (lower.find("food") != std::string::npos || lower.find("hunger") != std::string::npos) {
        unlockGuide("guide.food");
    }
    if (lower.find("goblin") != std::string::npos) unlockGuide("guide.monsters_goblins");
    if (lower.find("skeleton") != std::string::npos || lower.find("zombie") != std::string::npos || lower.find("undead") != std::string::npos) unlockGuide("guide.monsters_undead");
    if (lower.find("demon") != std::string::npos) unlockGuide("guide.monsters_demons");
    if (lower.find("dragon") != std::string::npos) unlockGuide("guide.monsters_dragons");
    if (lower.find("mind") != std::string::npos || lower.find("beholder") != std::string::npos || lower.find("aberration") != std::string::npos) unlockGuide("guide.monsters_aberrations");
    if (lower.find("biome") != std::string::npos) unlockGuide("guide.biomes");
    if (lower.find("infinite") != std::string::npos || lower.find("void") != std::string::npos) unlockGuide("guide.infinite_depth");
    if (lower.find("secret") != std::string::npos) unlockGuide("guide.secrets");
    if (lower.find("amulet") != std::string::npos || lower.find("shadowdeep") != std::string::npos) unlockGuide("guide.amulet");

    if (totalFragmentsCollected_ >= 5) unlockGuide("guide.biomes");
    if (totalFragmentsCollected_ >= 10) unlockGuide("guide.secrets");
    if (totalFragmentsCollected_ >= 20) unlockGuide("guide.infinite_depth");

    auto it = guides_.find(fragmentId);
    if (it != guides_.end()) {
        it->second.fragmentCount++;
        if (it->second.fragmentCount >= it->second.fragmentsRequired) {
            it->second.unlocked = true;
        }
    } else {
        for (auto& [gid, g] : guides_) {
            if (!sourceItem.empty() && g.sourceItem == sourceItem) {
                g.fragmentCount++;
                if (g.fragmentCount >= g.fragmentsRequired) g.unlocked = true;
            }
        }
    }
}

void GuideCodex::unlockGuide(const std::string& guideId) {
    auto it = guides_.find(guideId);
    if (it != guides_.end()) it->second.unlocked = true;
}

void GuideCodex::addLore(const LoreFragment& lore) {
    lores_[lore.id] = lore;
}

void GuideCodex::addLore(const std::string& id, const std::string& title, const std::string& text, int depth, bool bossDrop, const std::string& boss) {
    LoreFragment lf;
    lf.id = id;
    lf.title = title;
    lf.text = text;
    lf.depthFound = depth;
    lf.isBossDrop = bossDrop;
    lf.bossSource = boss;
    lores_[id] = lf;
}

bool GuideCodex::isUnlocked(const std::string& guideId) const {
    auto it = guides_.find(guideId);
    return it != guides_.end() && it->second.unlocked;
}

const GuideEntry* GuideCodex::getGuide(const std::string& id) const {
    auto it = guides_.find(id);
    if (it == guides_.end()) return nullptr;
    return &it->second;
}

std::vector<GuideEntry> GuideCodex::allGuides() const {
    std::vector<GuideEntry> out;
    out.reserve(guides_.size());
    for (auto& kv : guides_) out.push_back(kv.second);
    return out;
}

std::vector<GuideEntry> GuideCodex::unlockedGuides() const {
    std::vector<GuideEntry> out;
    for (auto& kv : guides_) if (kv.second.unlocked) out.push_back(kv.second);
    return out;
}

std::vector<GuideEntry> GuideCodex::guidesByCategory(GuideCategory cat) const {
    std::vector<GuideEntry> out;
    for (auto& kv : guides_) if (kv.second.category == cat) out.push_back(kv.second);
    return out;
}

const LoreFragment* GuideCodex::getLore(const std::string& id) const {
    auto it = lores_.find(id);
    if (it == lores_.end()) return nullptr;
    return &it->second;
}

std::vector<LoreFragment> GuideCodex::allLore() const {
    std::vector<LoreFragment> out;
    out.reserve(lores_.size());
    for (auto& kv : lores_) out.push_back(kv.second);
    return out;
}

std::vector<LoreFragment> GuideCodex::bossLore() const {
    std::vector<LoreFragment> out;
    for (auto& kv : lores_) if (kv.second.isBossDrop) out.push_back(kv.second);
    return out;
}

int GuideCodex::totalFragments() const {
    return totalFragmentsCollected_;
}

int GuideCodex::totalGuides() const {
    return static_cast<int>(guides_.size());
}

int GuideCodex::unlockedCount() const {
    int c = 0;
    for (auto& kv : guides_) if (kv.second.unlocked) c++;
    return c;
}

int GuideCodex::loreCount() const {
    return static_cast<int>(lores_.size());
}

std::string GuideCodex::categoryName(GuideCategory cat) const {
    switch (cat) {
        case GuideCategory::Weapons: return "Weapons";
        case GuideCategory::Armor: return "Armor";
        case GuideCategory::Potions: return "Potions";
        case GuideCategory::Scrolls: return "Scrolls";
        case GuideCategory::Monsters: return "Monsters";
        case GuideCategory::Biomes: return "Biomes";
        case GuideCategory::Lore: return "Lore";
        case GuideCategory::Secrets: return "Secrets";
        default: return "Unknown";
    }
}

}
