#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../core/vec2.hpp"
#include "../terminal/color.hpp"
#include "../combat/damage.hpp"

namespace shadowdeep {

enum class ItemKind : uint8_t {
    None = 0,
    Gold = 1,
    Ruby = 2,
    PotionHeal = 3,
    PotionStrength = 4,
    PotionHaste = 5,
    PotionRejuvenation = 6,
    PotionInvisibility = 7,
    Food = 8,
    ScrollLightning = 9,
    ScrollFireball = 10,
    ScrollTeleport = 11,
    ScrollMapping = 12,
    ScrollIdentify = 13,
    Weapon = 14,
    Armor = 15,
    Shield = 16,
    Helmet = 17,
    Boots = 18,
    Ring = 19,
    Amulet = 20,
    Key = 21,
    Bomb = 22,
    AmuletShadowdeep = 23
};

enum class ItemRarity : uint8_t {
    Common = 0,
    Uncommon = 1,
    Rare = 2,
    Epic = 3,
    Legendary = 4,
    Mythic = 5
};

enum class EquipSlot : uint8_t {
    None = 0,
    MainHand = 1,
    OffHand = 2,
    Body = 3,
    Head = 4,
    Feet = 5,
    Ring1 = 6,
    Ring2 = 7,
    Amulet = 8
};

enum class EnchantmentId : uint16_t {
    None = 0,
    Flaming = 1,
    Frostbound = 2,
    Stormforged = 3,
    Venomous = 4,
    Vampiric = 5,
    Swift = 6,
    Fortified = 7,
    Lucky = 8,
    Radiant = 9,
    ShadowTouched = 10,
    Thorned = 11,
    Echoing = 12,
    Count = 13
};

struct Enchantment {
    EnchantmentId id = EnchantmentId::None;
    int power = 0;

    std::string stableId() const;
    std::string displayName() const;
};

struct Item {
    std::string stableId;
    std::string name;
    ItemKind kind = ItemKind::None;
    ItemRarity rarity = ItemRarity::Common;
    char glyph = '?';
    Color color = Color::White;
    int power = 0;
    int valueGold = 0;
    int valueRubies = 0;
    EquipSlot slot = EquipSlot::None;
    DamageType damageType = DamageType::Physical;
    std::vector<Enchantment> enchantments;
    Vec2 pos{0, 0};
    bool identified = true;
    int count = 1;

    bool isEquipment() const;
    bool isConsumable() const;
    bool isStackable() const;
    std::string fullName() const;
};

struct ItemTemplate {
    std::string stableId;
    std::string name;
    ItemKind kind;
    ItemRarity rarity;
    char glyph;
    Color color;
    int minDepth;
    int maxDepth;
    int basePower;
    EquipSlot slot;
};

const std::vector<ItemTemplate>& allItemTemplates();

}
