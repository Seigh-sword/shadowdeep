#include "shadowdeep/shops/shop.hpp"
#include "shadowdeep/core/rng.hpp"
#include "shadowdeep/items/item.hpp"

namespace shadowdeep {

void Shop::generateStock(int depth, Rng& rng) {
    stock.clear();
    int count = 6 + rng.range(0, 4);

    for (int i = 0; i < count; ++i) {
        auto& templates = allItemTemplates();
        if (templates.empty()) break;
        auto& t = templates[rng.range(0, static_cast<int>(templates.size()) - 1)];
        Item item;
        item.stableId = t.stableId;
        item.name = t.name;
        item.kind = t.kind;
        item.glyph = t.glyph;
        item.color = t.color;
        item.power = t.basePower + rng.range(0, 2);
        item.slot = t.slot;
        item.identified = true;
        item.valueGold = 10 + depth * 5 + item.power * 8;

        ShopItem si;
        si.item = item;
        si.priceGold = item.valueGold;
        si.priceRubies = 0;
        if (item.rarity >= ItemRarity::Rare) si.priceRubies = 1;

        stock.push_back(si);
    }
}

bool Shop::buy(size_t idx, int& playerGold, int& playerRubies, Item& out) {
    if (idx >= stock.size()) return false;
    auto& si = stock[idx];
    if (si.sold) return false;
    if (playerGold < si.priceGold) return false;
    if (playerRubies < si.priceRubies) return false;

    playerGold -= si.priceGold;
    playerRubies -= si.priceRubies;
    out = si.item;
    si.sold = true;
    return true;
}

bool Shop::sell(const Item& item, int& playerGold) {
    int price = item.valueGold / 2;
    if (price < 1) price = 1;
    playerGold += price;
    return true;
}

}
