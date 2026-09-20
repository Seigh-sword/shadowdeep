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
    return kind == ItemKind::Weapon || kind == ItemKind::Armor || kind == ItemKind::Shield || kind == ItemKind::Helmet || kind == ItemKind::Boots || kind == ItemKind::Ring || kind == ItemKind::Amulet;
}

bool Item::isConsumable() const {
    return kind == ItemKind::PotionHeal || kind == ItemKind::PotionStrength || kind == ItemKind::PotionHaste || kind == ItemKind::PotionRejuvenation || kind == ItemKind::PotionInvisibility || kind == ItemKind::Food || kind == ItemKind::ScrollLightning || kind == ItemKind::ScrollFireball || kind == ItemKind::ScrollTeleport || kind == ItemKind::ScrollMapping || kind == ItemKind::ScrollIdentify || kind == ItemKind::Bomb;
}

bool Item::isStackable() const {
    return kind == ItemKind::Gold || kind == ItemKind::Ruby || kind == ItemKind::Food;
}

std::string Item::fullName() const {
    std::string prefix;
    if (!enchantments.empty()) {
        prefix = enchantments.front().displayName() + " ";
    }
    std::string rarityStr;
    switch (rarity) {
        case ItemRarity::Uncommon: rarityStr = ""; break;
        case ItemRarity::Rare: rarityStr = ""; break;
        case ItemRarity::Epic: rarityStr = ""; break;
        case ItemRarity::Legendary: rarityStr = ""; break;
        case ItemRarity::Mythic: rarityStr = ""; break;
        default: break;
    }
    std::string result = prefix + name;
    if (power > 0 && isEquipment()) {
        result += " +" + std::to_string(power);
    }
    return result;
}

const std::vector<ItemTemplate>& allItemTemplates() {
    static std::vector<ItemTemplate> templates = {
        {"item.dagger", "dagger", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 1, 10, 2, EquipSlot::MainHand},
        {"item.short_sword", "short sword", ItemKind::Weapon, ItemRarity::Common, '/', Color::Steel, 2, 12, 4, EquipSlot::MainHand},
        {"item.battle_axe", "battle axe", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 5, 20, 6, EquipSlot::MainHand},
        {"item.runed_greatsword", "runed greatsword", ItemKind::Weapon, ItemRarity::Rare, '/', Color::BrightWhite, 8, 30, 9, EquipSlot::MainHand},
        {"item.warhammer", "warhammer", ItemKind::Weapon, ItemRarity::Uncommon, '/', Color::Steel, 6, 22, 7, EquipSlot::MainHand},
        {"item.leather_armour", "leather armour", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 1, 10, 1, EquipSlot::Body},
        {"item.chain_mail", "chain mail", ItemKind::Armor, ItemRarity::Common, '[', Color::Steel, 3, 15, 3, EquipSlot::Body},
        {"item.plate_armour", "plate armour", ItemKind::Armor, ItemRarity::Uncommon, '[', Color::Steel, 6, 25, 5, EquipSlot::Body},
        {"item.dragon_scale_mail", "dragon scale mail", ItemKind::Armor, ItemRarity::Legendary, '[', Color::Gold, 8, 30, 8, EquipSlot::Body},
        {"item.buckler", "buckler", ItemKind::Shield, ItemRarity::Common, '[', Color::Steel, 1, 12, 1, EquipSlot::OffHand},
        {"item.helmet", "iron helmet", ItemKind::Helmet, ItemRarity::Common, '[', Color::Steel, 2, 14, 1, EquipSlot::Head},
        {"item.boots_swift", "swift boots", ItemKind::Boots, ItemRarity::Uncommon, '[', Color::Steel, 3, 18, 1, EquipSlot::Feet},
        {"item.ring_haste", "ring of haste", ItemKind::Ring, ItemRarity::Rare, '=', Color::Gold, 4, 25, 2, EquipSlot::Ring1},
        {"item.amulet_protection", "amulet of protection", ItemKind::Amulet, ItemRarity::Rare, '"', Color::Gold, 5, 25, 2, EquipSlot::Amulet},
    };
    return templates;
}

}
