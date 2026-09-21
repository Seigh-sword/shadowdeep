#include "shadowdeep/items/item.hpp"

namespace shadowdeep {

std::string Enchantment::stableId() const {
    switch (id) {
        case EnchantmentId::Flaming: return "ench.flaming";
        case EnchantmentId::Frostbound: return "ench.frostbound";
        case EnchantmentId::Stormforged: return "ench.stormforged";
        case EnchantmentId::Venomous: return "ench.venomous";
        case EnchantmentId::Vampiric: return "ench.vampiric";
        case EnchantmentId::Swift: return "ench.swift";
        case EnchantmentId::Fortified: return "ench.fortified";
        case EnchantmentId::Lucky: return "ench.lucky";
        case EnchantmentId::Radiant: return "ench.radiant";
        case EnchantmentId::ShadowTouched: return "ench.shadow_touched";
        case EnchantmentId::Thorned: return "ench.thorned";
        case EnchantmentId::Echoing: return "ench.echoing";
        default: return "ench.none";
    }
}

std::string Enchantment::displayName() const {
    switch (id) {
        case EnchantmentId::Flaming: return "Flaming";
        case EnchantmentId::Frostbound: return "Frostbound";
        case EnchantmentId::Stormforged: return "Stormforged";
        case EnchantmentId::Venomous: return "Venomous";
        case EnchantmentId::Vampiric: return "Vampiric";
        case EnchantmentId::Swift: return "Swift";
        case EnchantmentId::Fortified: return "Fortified";
        case EnchantmentId::Lucky: return "Lucky";
        case EnchantmentId::Radiant: return "Radiant";
        case EnchantmentId::ShadowTouched: return "Shadow-touched";
        case EnchantmentId::Thorned: return "Thorned";
        case EnchantmentId::Echoing: return "Echoing";
        default: return "None";
    }
}

bool Item::isEquipment() const {
    return kind == ItemKind::Weapon || kind == ItemKind::Armor || kind == ItemKind::Shield || kind == ItemKind::Helmet || kind == ItemKind::Boots || kind == ItemKind::Ring || kind == ItemKind::Amulet || kind == ItemKind::WeaponRanged;
}

bool Item::isConsumable() const {
    return kind == ItemKind::PotionHeal || kind == ItemKind::PotionGreaterHeal || kind == ItemKind::PotionStrength || kind == ItemKind::PotionHaste || kind == ItemKind::PotionRejuvenation || kind == ItemKind::PotionInvisibility || kind == ItemKind::PotionMana || kind == ItemKind::PotionAntidote || kind == ItemKind::PotionFireResist || kind == ItemKind::PotionFrostResist || kind == ItemKind::PotionShadowResist || kind == ItemKind::Food || kind == ItemKind::FoodBread || kind == ItemKind::FoodMeat || kind == ItemKind::FoodRation || kind == ItemKind::FoodFruit || kind == ItemKind::ScrollLightning || kind == ItemKind::ScrollFireball || kind == ItemKind::ScrollTeleport || kind == ItemKind::ScrollMapping || kind == ItemKind::ScrollIdentify || kind == ItemKind::ScrollEnchant || kind == ItemKind::ScrollBanishment || kind == ItemKind::ScrollSummon || kind == ItemKind::Bomb || kind == ItemKind::BombDynamite;
}

bool Item::isStackable() const {
    return kind == ItemKind::Gold || kind == ItemKind::Ruby || kind == ItemKind::Food || kind == ItemKind::FoodBread || kind == ItemKind::FoodMeat || kind == ItemKind::FoodRation || kind == ItemKind::FoodFruit || kind == ItemKind::GuideFragment;
}

std::string Item::fullName() const {
    std::string prefix;
    if (!enchantments.empty()) {
        prefix = enchantments.front().displayName() + " ";
    }
    std::string result = prefix + name;
    if (power > 0 && isEquipment()) {
        result += std::string(" +") + std::to_string(power);
    }
    if (count > 1 && isStackable()) {
        result += std::string(" x") + std::to_string(count);
    }
    return result;
}

const std::vector<ItemTemplate>& allItemTemplates() {
    static std::vector<ItemTemplate> templates = {
        {"item.dagger", "dagger", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 1, 10, 2, EquipSlot::MainHand},
        {"item.short_sword", "short sword", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 2, 12, 4, EquipSlot::MainHand},
        {"item.longsword", "longsword", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 3, 15, 5, EquipSlot::MainHand},
        {"item.broadsword", "broadsword", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 4, 16, 6, EquipSlot::MainHand},
        {"item.battle_axe", "battle axe", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 20, 6, EquipSlot::MainHand},
        {"item.war_axe", "war axe", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 22, 7, EquipSlot::MainHand},
        {"item.greataxe", "greataxe", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightWhite, 7, 28, 9, EquipSlot::MainHand},
        {"item.double_axe", "double axe", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 8, 30, 10, EquipSlot::MainHand},
        {"item.runed_greatsword", "runed greatsword", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightWhite, 8, 30, 9, EquipSlot::MainHand},
        {"item.claymore", "claymore", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 8, 32, 11, EquipSlot::MainHand},
        {"item.katana", "katana", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightCyan, 7, 30, 9, EquipSlot::MainHand},
        {"item.wakizashi", "wakizashi", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 25, 6, EquipSlot::MainHand},
        {"item.scimitar", "scimitar", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 24, 7, EquipSlot::MainHand},
        {"item.falchion", "falchion", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 26, 7, EquipSlot::MainHand},
        {"item.rapier", "rapier", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::BrightWhite, 5, 25, 6, EquipSlot::MainHand},
        {"item.estoc", "estoc", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 7, 29, 8, EquipSlot::MainHand},
        {"item.cutlass", "cutlass", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 4, 20, 5, EquipSlot::MainHand},
        {"item.sabre", "sabre", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 23, 6, EquipSlot::MainHand},
        {"item.warhammer", "warhammer", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 22, 7, EquipSlot::MainHand},
        {"item.maul", "maul", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 8, 32, 10, EquipSlot::MainHand},
        {"item.great_hammer", "great hammer", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 9, 35, 12, EquipSlot::MainHand},
        {"item.mace", "mace", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 3, 18, 5, EquipSlot::MainHand},
        {"item.morning_star", "morning star", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 26, 8, EquipSlot::MainHand},
        {"item.flail", "flail", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 27, 8, EquipSlot::MainHand},
        {"item.quarterstaff", "quarterstaff", ItemKind::Weapon, ItemRarity::Common, '/', Color::Brown, 2, 15, 4, EquipSlot::MainHand},
        {"item.bo_staff", "bo staff", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Brown, 4, 22, 6, EquipSlot::MainHand},
        {"item.nunchaku", "nunchaku", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Brown, 5, 24, 6, EquipSlot::MainHand},
        {"item.sai", "sai", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 4, 20, 5, EquipSlot::MainHand},
        {"item.katar", "katar", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 6, 28, 7, EquipSlot::MainHand},
        {"item.claw", "claw", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightRed, 7, 30, 8, EquipSlot::MainHand},
        {"item.whip", "whip", ItemKind::Weapon, ItemRarity::Common, '/', Color::Brown, 3, 18, 4, EquipSlot::MainHand},
        {"item.chain_whip", "chain whip", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 28, 7, EquipSlot::MainHand},
        {"item.halberd", "halberd", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 7, 32, 9, EquipSlot::MainHand},
        {"item.glaive", "glaive", ItemKind::Weapon, ItemRarity::Rare, '/', Color::Steel, 7, 33, 9, EquipSlot::MainHand},
        {"item.spear", "spear", ItemKind::Weapon, ItemRarity::Common, '/', Color::Brown, 3, 18, 5, EquipSlot::MainHand},
        {"item.pike", "pike", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Brown, 5, 25, 7, EquipSlot::MainHand},
        {"item.trident", "trident", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightCyan, 7, 30, 8, EquipSlot::MainHand},
        {"item.longbow", "longbow", ItemKind::WeaponRanged, ItemRarity::Common, ')', Color::Brown, 3, 20, 5, EquipSlot::MainHand},
        {"item.shortbow", "shortbow", ItemKind::WeaponRanged, ItemRarity::Common, ')', Color::Brown, 2, 18, 4, EquipSlot::MainHand},
        {"item.recurve_bow", "recurve bow", ItemKind::WeaponRanged, ItemRarity::Uncommon, ')', Color::Brown, 5, 25, 7, EquipSlot::MainHand},
        {"item.crossbow", "crossbow", ItemKind::WeaponRanged, ItemRarity::Uncommon, ')', Color::Steel, 5, 26, 7, EquipSlot::MainHand},
        {"item.heavy_crossbow", "heavy crossbow", ItemKind::WeaponRanged, ItemRarity::Rare, ')', Color::Steel, 8, 32, 10, EquipSlot::MainHand},
        {"item.sling", "sling", ItemKind::WeaponRanged, ItemRarity::Common, ')', Color::Brown, 1, 15, 3, EquipSlot::MainHand},
        {"item.javelin", "javelin", ItemKind::WeaponThrown, ItemRarity::Common, '/', Color::Brown, 2, 18, 4, EquipSlot::MainHand},
        {"item.throwing_axe", "throwing axe", ItemKind::WeaponThrown, ItemRarity::Common, '/', Color::Steel, 3, 20, 5, EquipSlot::MainHand},
        {"item.throwing_knife", "throwing knife", ItemKind::WeaponThrown, ItemRarity::Common, '/', Color::Steel, 2, 16, 4, EquipSlot::MainHand},
        {"item.wand_sparks", "wand of sparks", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::BrightYellow, 4, 22, 6, EquipSlot::MainHand},
        {"item.wand_fire", "wand of fire", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightRed, 6, 28, 8, EquipSlot::MainHand},
        {"item.staff_frost", "staff of frost", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightCyan, 6, 30, 8, EquipSlot::MainHand},
        {"item.staff_storm", "staff of storm", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightYellow, 7, 32, 9, EquipSlot::MainHand},
        {"item.staff_shadow", "staff of shadow", ItemKind::Weapon, ItemRarity::Epic, '/', Color::Purple, 9, 40, 12, EquipSlot::MainHand},
        {"item.demon_blade", "demon blade", ItemKind::Weapon, ItemRarity::Epic, '/', Color::BrightRed, 10, 45, 14, EquipSlot::MainHand},
        {"item.angel_blade", "angel blade", ItemKind::Weapon, ItemRarity::Legendary, '/', Color::BrightWhite, 12, 50, 16, EquipSlot::MainHand},
        {"item.shadowdeep_edge", "shadowdeep edge", ItemKind::Weapon, ItemRarity::Mythic, '/', Color::Gold, 15, 99, 20, EquipSlot::MainHand},

        {"item.leather_armour", "leather armour", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 1, 10, 1, EquipSlot::Body},
        {"item.padded_armour", "padded armour", ItemKind::Armor, ItemRarity::Common, '[', Color::Brown, 1, 12, 1, EquipSlot::Body},
        {"item.studded_leather", "studded leather", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 2, 14, 2, EquipSlot::Body},
        {"item.hide_armour", "hide armour", ItemKind::Armor, ItemRarity::Common, '[', Color::Brown, 2, 15, 2, EquipSlot::Body},
        {"item.scale_mail", "scale mail", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 3, 18, 3, EquipSlot::Body},
        {"item.chain_mail", "chain mail", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 3, 15, 3, EquipSlot::Body},
        {"item.chain_hauberk", "chainmail hauberk", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 4, 20, 4, EquipSlot::Body},
        {"item.splint_mail", "splint mail", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 5, 22, 4, EquipSlot::Body},
        {"item.brigandine", "brigandine", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 5, 24, 5, EquipSlot::Body},
        {"item.half_plate", "half plate", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 6, 28, 5, EquipSlot::Body},
        {"item.plate_armour", "plate armour", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 6, 25, 5, EquipSlot::Body},
        {"item.mithril_chain", "mithril chain", ItemKind::Armor, ItemRarity::Rare, '[', Color::BrightCyan, 7, 35, 6, EquipSlot::Body},
        {"item.adamantine_plate", "adamantine plate", ItemKind::Armor, ItemRarity::Epic, '[', Color::Steel, 9, 45, 8, EquipSlot::Body},
        {"item.dragon_scale_mail", "dragon scale mail", ItemKind::Armor, ItemRarity::Legendary, '[', Color::Gold, 8, 30, 8, EquipSlot::Body},
        {"item.shadow_plate", "shadow plate", ItemKind::Armor, ItemRarity::Legendary, '[', Color::Purple, 10, 50, 10, EquipSlot::Body},
        {"item.elven_cloak", "elven cloak", ItemKind::Armor, ItemRarity::Rare, '[', Color::BrightGreen, 5, 30, 3, EquipSlot::Body},
        {"item.robe_archmagi", "robe of the archmagi", ItemKind::Armor, ItemRarity::Epic, '[', Color::BrightMagenta, 8, 45, 5, EquipSlot::Body},
        {"item.robe_shadow", "robe of shadow", ItemKind::Armor, ItemRarity::Legendary, '[', Color::Purple, 10, 60, 7, EquipSlot::Body},

        {"item.buckler", "buckler", ItemKind::Shield, ItemRarity::Common, '[', Color::Steel, 1, 12, 1, EquipSlot::OffHand},
        {"item.wooden_shield", "wooden shield", ItemKind::Shield, ItemRarity::Common, '[', Color::Brown, 1, 14, 1, EquipSlot::OffHand},
        {"item.iron_shield", "iron shield", ItemKind::Shield, ItemRarity::Common, '[', Color::Steel, 3, 18, 2, EquipSlot::OffHand},
        {"item.steel_shield", "steel shield", ItemKind::Shield, ItemRarity::Uncommon, '[', Color::Steel, 5, 24, 3, EquipSlot::OffHand},
        {"item.tower_shield", "tower shield", ItemKind::Shield, ItemRarity::Uncommon, '[', Color::Steel, 6, 28, 4, EquipSlot::OffHand},
        {"item.kite_shield", "kite shield", ItemKind::Shield, ItemRarity::Uncommon, '[', Color::Steel, 5, 26, 3, EquipSlot::OffHand},
        {"item.spiked_shield", "spiked shield", ItemKind::Shield, ItemRarity::Rare, '[', Color::Steel, 7, 35, 5, EquipSlot::OffHand},
        {"item.mirror_shield", "mirror shield", ItemKind::Shield, ItemRarity::Rare, '[', Color::BrightWhite, 8, 40, 5, EquipSlot::OffHand},
        {"item.dragon_shield", "dragon shield", ItemKind::Shield, ItemRarity::Legendary, '[', Color::Gold, 10, 60, 8, EquipSlot::OffHand},

        {"item.helmet", "iron helmet", ItemKind::Helmet, ItemRarity::Common, '[', Color::Steel, 2, 14, 1, EquipSlot::Head},
        {"item.cap", "cloth cap", ItemKind::Helmet, ItemRarity::Common, '[', Color::Brown, 1, 10, 0, EquipSlot::Head},
        {"item.leather_cap", "leather cap", ItemKind::Helmet, ItemRarity::Common, '[', Color::Brown, 1, 12, 1, EquipSlot::Head},
        {"item.steel_helm", "steel helm", ItemKind::Helmet, ItemRarity::Uncommon, '[', Color::Steel, 4, 22, 2, EquipSlot::Head},
        {"item.great_helm", "great helm", ItemKind::Helmet, ItemRarity::Uncommon, '[', Color::Steel, 6, 28, 3, EquipSlot::Head},
        {"item.crown", "crown of sight", ItemKind::Helmet, ItemRarity::Rare, '[', Color::Gold, 7, 35, 2, EquipSlot::Head},
        {"item.helm_dragonsight", "helm of dragonsight", ItemKind::Helmet, ItemRarity::Legendary, '[', Color::Gold, 10, 60, 5, EquipSlot::Head},
        {"item.hood_shadow", "hood of shadow", ItemKind::Helmet, ItemRarity::Epic, '[', Color::Purple, 9, 50, 4, EquipSlot::Head},

        {"item.boots_swift", "swift boots", ItemKind::Boots, ItemRarity::Uncommon, '[', Color::Steel, 3, 18, 1, EquipSlot::Feet},
        {"item.sandals", "sandals", ItemKind::Boots, ItemRarity::Common, '[', Color::Brown, 1, 10, 0, EquipSlot::Feet},
        {"item.leather_boots", "leather boots", ItemKind::Boots, ItemRarity::Common, '[', Color::Brown, 1, 14, 1, EquipSlot::Feet},
        {"item.iron_boots", "iron boots", ItemKind::Boots, ItemRarity::Common, '[', Color::Steel, 3, 20, 2, EquipSlot::Feet},
        {"item.boots_travel", "boots of travel", ItemKind::Boots, ItemRarity::Rare, '[', Color::BrightCyan, 6, 35, 2, EquipSlot::Feet},
        {"item.boots_levitation", "boots of levitation", ItemKind::Boots, ItemRarity::Rare, '[', Color::BrightWhite, 7, 40, 3, EquipSlot::Feet},
        {"item.boots_shadowstep", "boots of shadowstep", ItemKind::Boots, ItemRarity::Epic, '[', Color::Purple, 9, 60, 4, EquipSlot::Feet},

        {"item.ring_haste", "ring of haste", ItemKind::Ring, ItemRarity::Rare, '=', Color::Gold, 4, 25, 2, EquipSlot::Ring1},
        {"item.ring_protection", "ring of protection", ItemKind::Ring, ItemRarity::Uncommon, '=', Color::Gold, 3, 22, 2, EquipSlot::Ring1},
        {"item.ring_strength", "ring of strength", ItemKind::Ring, ItemRarity::Uncommon, '=', Color::BrightRed, 3, 25, 2, EquipSlot::Ring1},
        {"item.ring_dexterity", "ring of dexterity", ItemKind::Ring, ItemRarity::Uncommon, '=', Color::BrightGreen, 3, 25, 2, EquipSlot::Ring1},
        {"item.ring_intelligence", "ring of intelligence", ItemKind::Ring, ItemRarity::Uncommon, '=', Color::BrightCyan, 3, 25, 2, EquipSlot::Ring1},
        {"item.ring_regeneration", "ring of regeneration", ItemKind::Ring, ItemRarity::Rare, '=', Color::BrightMagenta, 5, 35, 3, EquipSlot::Ring1},
        {"item.ring_fire_resist", "ring of fire resistance", ItemKind::Ring, ItemRarity::Rare, '=', Color::BrightRed, 5, 35, 2, EquipSlot::Ring1},
        {"item.ring_frost_resist", "ring of frost resistance", ItemKind::Ring, ItemRarity::Rare, '=', Color::BrightCyan, 5, 35, 2, EquipSlot::Ring1},
        {"item.ring_poison_resist", "ring of poison resistance", ItemKind::Ring, ItemRarity::Rare, '=', Color::Green, 5, 35, 2, EquipSlot::Ring1},
        {"item.ring_shadow_resist", "ring of shadow resistance", ItemKind::Ring, ItemRarity::Epic, '=', Color::Purple, 8, 50, 3, EquipSlot::Ring1},
        {"item.ring_luck", "ring of luck", ItemKind::Ring, ItemRarity::Epic, '=', Color::Gold, 7, 45, 2, EquipSlot::Ring1},
        {"item.ring_invisibility", "ring of invisibility", ItemKind::Ring, ItemRarity::Epic, '=', Color::BrightWhite, 8, 55, 2, EquipSlot::Ring1},

        {"item.amulet_protection", "amulet of protection", ItemKind::Amulet, ItemRarity::Rare, '"', Color::Gold, 5, 25, 2, EquipSlot::Amulet},
        {"item.amulet_health", "amulet of health", ItemKind::Amulet, ItemRarity::Rare, '"', Color::BrightRed, 5, 30, 3, EquipSlot::Amulet},
        {"item.amulet_mana", "amulet of mana", ItemKind::Amulet, ItemRarity::Rare, '"', Color::BrightCyan, 5, 30, 2, EquipSlot::Amulet},
        {"item.amulet_shadows", "amulet of shadows", ItemKind::Amulet, ItemRarity::Epic, '"', Color::Purple, 8, 50, 4, EquipSlot::Amulet},
        {"item.amulet_dragons", "amulet of dragons", ItemKind::Amulet, ItemRarity::Legendary, '"', Color::Gold, 10, 70, 6, EquipSlot::Amulet},
        {"item.amulet_shadowdeep", "Amulet of Shadowdeep", ItemKind::AmuletShadowdeep, ItemRarity::Mythic, '"', Color::Gold, 25, 99, 10, EquipSlot::Amulet},

        {"item.potion_heal", "potion of healing", ItemKind::PotionHeal, ItemRarity::Common, '!', Color::BrightRed, 1, 99, 0, EquipSlot::None},
        {"item.potion_greater_heal", "potion of greater healing", ItemKind::PotionGreaterHeal, ItemRarity::Uncommon, '!', Color::BrightRed, 5, 99, 0, EquipSlot::None},
        {"item.potion_strength", "potion of strength", ItemKind::PotionStrength, ItemRarity::Uncommon, '!', Color::BrightYellow, 3, 99, 0, EquipSlot::None},
        {"item.potion_haste", "potion of haste", ItemKind::PotionHaste, ItemRarity::Uncommon, '!', Color::BrightCyan, 4, 99, 0, EquipSlot::None},
        {"item.potion_rejuvenation", "potion of rejuvenation", ItemKind::PotionRejuvenation, ItemRarity::Rare, '!', Color::BrightMagenta, 6, 99, 0, EquipSlot::None},
        {"item.potion_invisibility", "potion of invisibility", ItemKind::PotionInvisibility, ItemRarity::Rare, '!', Color::BrightWhite, 6, 99, 0, EquipSlot::None},
        {"item.potion_mana", "potion of mana", ItemKind::PotionMana, ItemRarity::Uncommon, '!', Color::BrightCyan, 4, 99, 0, EquipSlot::None},
        {"item.potion_antidote", "potion of antidote", ItemKind::PotionAntidote, ItemRarity::Common, '!', Color::Green, 2, 99, 0, EquipSlot::None},
        {"item.potion_fire_resist", "potion of fire resistance", ItemKind::PotionFireResist, ItemRarity::Uncommon, '!', Color::BrightRed, 5, 99, 0, EquipSlot::None},
        {"item.potion_frost_resist", "potion of frost resistance", ItemKind::PotionFrostResist, ItemRarity::Uncommon, '!', Color::BrightCyan, 5, 99, 0, EquipSlot::None},
        {"item.potion_shadow_resist", "potion of shadow resistance", ItemKind::PotionShadowResist, ItemRarity::Rare, '!', Color::Purple, 7, 99, 0, EquipSlot::None},

        {"item.food", "food ration", ItemKind::Food, ItemRarity::Common, '%', Color::Brown, 1, 99, 0, EquipSlot::None},
        {"item.bread", "bread", ItemKind::FoodBread, ItemRarity::Common, '%', Color::Brown, 1, 99, 0, EquipSlot::None},
        {"item.meat", "meat", ItemKind::FoodMeat, ItemRarity::Common, '%', Color::BrightRed, 1, 99, 0, EquipSlot::None},
        {"item.ration", "iron ration", ItemKind::FoodRation, ItemRarity::Common, '%', Color::Steel, 2, 99, 0, EquipSlot::None},
        {"item.fruit", "fruit", ItemKind::FoodFruit, ItemRarity::Common, '%', Color::BrightGreen, 1, 99, 0, EquipSlot::None},
        {"item.cheese", "cheese", ItemKind::Food, ItemRarity::Common, '%', Color::Yellow, 1, 99, 0, EquipSlot::None},
        {"item.jerky", "jerky", ItemKind::FoodMeat, ItemRarity::Common, '%', Color::Brown, 2, 99, 0, EquipSlot::None},
        {"item.soup", "soup", ItemKind::Food, ItemRarity::Uncommon, '%', Color::BrightYellow, 3, 99, 0, EquipSlot::None},
        {"item.feast", "feast", ItemKind::FoodRation, ItemRarity::Rare, '%', Color::Gold, 5, 99, 0, EquipSlot::None},

        {"item.scroll_lightning", "scroll of lightning", ItemKind::ScrollLightning, ItemRarity::Common, '?', Color::BrightYellow, 1, 99, 0, EquipSlot::None},
        {"item.scroll_fireball", "scroll of fireball", ItemKind::ScrollFireball, ItemRarity::Uncommon, '?', Color::BrightRed, 3, 99, 0, EquipSlot::None},
        {"item.scroll_teleport", "scroll of teleport", ItemKind::ScrollTeleport, ItemRarity::Uncommon, '?', Color::BrightMagenta, 3, 99, 0, EquipSlot::None},
        {"item.scroll_mapping", "scroll of mapping", ItemKind::ScrollMapping, ItemRarity::Common, '?', Color::BrightCyan, 2, 99, 0, EquipSlot::None},
        {"item.scroll_identify", "scroll of identify", ItemKind::ScrollIdentify, ItemRarity::Common, '?', Color::White, 1, 99, 0, EquipSlot::None},
        {"item.scroll_enchant", "scroll of enchantment", ItemKind::ScrollEnchant, ItemRarity::Rare, '?', Color::Gold, 5, 99, 0, EquipSlot::None},
        {"item.scroll_banishment", "scroll of banishment", ItemKind::ScrollBanishment, ItemRarity::Rare, '?', Color::Purple, 6, 99, 0, EquipSlot::None},
        {"item.scroll_summon", "scroll of summoning", ItemKind::ScrollSummon, ItemRarity::Epic, '?', Color::BrightMagenta, 8, 99, 0, EquipSlot::None},

        {"item.bomb", "bomb", ItemKind::Bomb, ItemRarity::Uncommon, '*', Color::BrightRed, 3, 99, 0, EquipSlot::None},
        {"item.dynamite", "dynamite", ItemKind::BombDynamite, ItemRarity::Rare, '*', Color::BrightRed, 6, 99, 0, EquipSlot::None},
        {"item.key", "iron key", ItemKind::Key, ItemRarity::Common, '-', Color::Steel, 1, 99, 0, EquipSlot::None},
        {"item.gold_key", "gold key", ItemKind::Key, ItemRarity::Uncommon, '-', Color::Gold, 5, 99, 0, EquipSlot::None},
        {"item.shadow_key", "shadow key", ItemKind::Key, ItemRarity::Rare, '-', Color::Purple, 10, 99, 0, EquipSlot::None},

        {"item.guide_fragment_common", "guide fragment", ItemKind::GuideFragment, ItemRarity::Common, ';', Color::White, 1, 99, 0, EquipSlot::None},
        {"item.guide_fragment_uncommon", "faded guide fragment", ItemKind::GuideFragment, ItemRarity::Uncommon, ';', Color::BrightCyan, 3, 99, 0, EquipSlot::None},
        {"item.guide_fragment_rare", "ancient guide fragment", ItemKind::GuideFragment, ItemRarity::Rare, ';', Color::Gold, 7, 99, 0, EquipSlot::None},
        {"item.guide_fragment_epic", "forbidden guide fragment", ItemKind::GuideFragment, ItemRarity::Epic, ';', Color::Purple, 12, 99, 0, EquipSlot::None},
        {"item.guide_fragment_legendary", "shadowdeep guide fragment", ItemKind::GuideFragment, ItemRarity::Legendary, ';', Color::Gold, 20, 99, 0, EquipSlot::None},
        {"item.lore_scroll", "lore scroll", ItemKind::LoreScroll, ItemRarity::Uncommon, '?', Color::BrightYellow, 2, 99, 0, EquipSlot::None},
        {"item.lore_tome", "tome of lore", ItemKind::LoreScroll, ItemRarity::Rare, '?', Color::Gold, 6, 99, 0, EquipSlot::None},
        {"item.lore_codex", "codex of shadowdeep", ItemKind::LoreScroll, ItemRarity::Legendary, '?', Color::Purple, 15, 99, 0, EquipSlot::None},
        {"item.tool_torch", "torch", ItemKind::Tool, ItemRarity::Common, '(', Color::BrightYellow, 1, 99, 0, EquipSlot::None},
        {"item.tool_rope", "rope", ItemKind::Tool, ItemRarity::Common, '(', Color::Brown, 1, 99, 0, EquipSlot::None},
        {"item.tool_pickaxe", "pickaxe", ItemKind::Tool, ItemRarity::Common, '(', Color::Steel, 2, 99, 0, EquipSlot::None},
    };
    return templates;
}

}
