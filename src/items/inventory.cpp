#include "shadowdeep/items/inventory.hpp"

namespace shadowdeep {

bool Inventory::add(const Item& item) {
    if (items_.size() >= kInventoryMax) return false;
    if (item.isStackable()) {
        for (auto& it : items_) {
            if (it.stableId == item.stableId && it.kind == item.kind) {
                it.count += item.count;
                return true;
            }
        }
    }
    items_.push_back(item);
    return true;
}

bool Inventory::remove(size_t idx) {
    if (idx >= items_.size()) return false;
    items_.erase(items_.begin() + static_cast<long>(idx));
    return true;
}

bool Inventory::hasSpace() const {
    return items_.size() < kInventoryMax;
}

size_t Inventory::size() const {
    return items_.size();
}

bool Inventory::empty() const {
    return items_.empty();
}

Item& Inventory::at(size_t idx) {
    return items_.at(idx);
}

const Item& Inventory::at(size_t idx) const {
    return items_.at(idx);
}

std::vector<Item>& Inventory::all() {
    return items_;
}

const std::vector<Item>& Inventory::all() const {
    return items_;
}

void Inventory::clear() {
    items_.clear();
}

std::optional<size_t> Inventory::findFirst(ItemKind kind) const {
    for (size_t i = 0; i < items_.size(); ++i) {
        if (items_[i].kind == kind) return i;
    }
    return std::nullopt;
}

int Inventory::countGold() const {
    int total = 0;
    for (auto& it : items_) if (it.kind == ItemKind::Gold) total += it.valueGold * it.count;
    return total;
}

int Inventory::countRubies() const {
    int total = 0;
    for (auto& it : items_) if (it.kind == ItemKind::Ruby) total += it.valueGold * it.count;
    return total;
}

int Equipment::totalArmor() const {
    int sum = 0;
    if (body) sum += body->power;
    if (head) sum += head->power;
    if (feet) sum += feet->power;
    if (offHand && offHand->kind == ItemKind::Shield) sum += offHand->power;
    if (ring1) sum += ring1->power / 2;
    if (ring2) sum += ring2->power / 2;
    if (amulet) sum += amulet->power;
    for (auto& e : allEnchantments()) {
        if (e.id == EnchantmentId::Fortified) sum += e.power;
    }
    return sum;
}

int Equipment::totalAttack() const {
    int sum = 0;
    if (mainHand) sum += mainHand->power;
    if (offHand && offHand->kind == ItemKind::Weapon) sum += offHand->power / 2;
    for (auto& e : allEnchantments()) {
        if (e.id == EnchantmentId::Flaming || e.id == EnchantmentId::Stormforged) sum += e.power;
    }
    return sum;
}

std::vector<Enchantment> Equipment::allEnchantments() const {
    std::vector<Enchantment> res;
    auto add = [&](const std::optional<Item>& it){
        if (it) for (auto& en : it->enchantments) res.push_back(en);
    };
    add(mainHand);
    add(offHand);
    add(body);
    add(head);
    add(feet);
    add(ring1);
    add(ring2);
    add(amulet);
    return res;
}

bool Equipment::equip(const Item& item, std::optional<Item>& previous) {
    switch (item.slot) {
        case EquipSlot::MainHand:
            previous = mainHand;
            mainHand = item;
            return true;
        case EquipSlot::OffHand:
            previous = offHand;
            offHand = item;
            return true;
        case EquipSlot::Body:
            previous = body;
            body = item;
            return true;
        case EquipSlot::Head:
            previous = head;
            head = item;
            return true;
        case EquipSlot::Feet:
            previous = feet;
            feet = item;
            return true;
        case EquipSlot::Ring1:
            if (!ring1) { previous = std::nullopt; ring1 = item; return true; }
            if (!ring2) { previous = std::nullopt; ring2 = item; return true; }
            previous = ring1;
            ring1 = item;
            return true;
        case EquipSlot::Ring2:
            previous = ring2;
            ring2 = item;
            return true;
        case EquipSlot::Amulet:
            previous = amulet;
            amulet = item;
            return true;
        default:
            return false;
    }
}

bool Equipment::unequip(EquipSlot slot, Item& out) {
    std::optional<Item>* target = nullptr;
    switch (slot) {
        case EquipSlot::MainHand: target = &mainHand; break;
        case EquipSlot::OffHand: target = &offHand; break;
        case EquipSlot::Body: target = &body; break;
        case EquipSlot::Head: target = &head; break;
        case EquipSlot::Feet: target = &feet; break;
        case EquipSlot::Ring1: target = &ring1; break;
        case EquipSlot::Ring2: target = &ring2; break;
        case EquipSlot::Amulet: target = &amulet; break;
        default: return false;
    }
    if (!*target) return false;
    out = **target;
    target->reset();
    return true;
}

}
