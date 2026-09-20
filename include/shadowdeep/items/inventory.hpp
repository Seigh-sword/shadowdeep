#pragma once
#include <vector>
#include <optional>
#include "item.hpp"

namespace shadowdeep {

constexpr int kInventoryMax = 26;
constexpr int kQuickSlots = 4;

class Inventory {
public:
    bool add(const Item& item);
    bool remove(size_t idx);
    bool hasSpace() const;
    size_t size() const;
    bool empty() const;
    Item& at(size_t idx);
    const Item& at(size_t idx) const;
    std::vector<Item>& all();
    const std::vector<Item>& all() const;
    void clear();

    std::optional<size_t> findFirst(ItemKind kind) const;
    int countGold() const;
    int countRubies() const;

private:
    std::vector<Item> items_;
};

struct Equipment {
    std::optional<Item> mainHand;
    std::optional<Item> offHand;
    std::optional<Item> body;
    std::optional<Item> head;
    std::optional<Item> feet;
    std::optional<Item> ring1;
    std::optional<Item> ring2;
    std::optional<Item> amulet;

    int totalArmor() const;
    int totalAttack() const;
    std::vector<Enchantment> allEnchantments() const;
    bool equip(const Item& item, std::optional<Item>& previous);
    bool unequip(EquipSlot slot, Item& out);
};

}
