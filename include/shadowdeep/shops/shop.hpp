#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../items/item.hpp"

namespace shadowdeep {

enum class ShopType : uint8_t {
    General = 0,
    Blacksmith = 1,
    Alchemist = 2,
    Enchanter = 3,
    Wandering = 4
};

struct ShopItem {
    Item item;
    int priceGold = 0;
    int priceRubies = 0;
    bool sold = false;
};

struct Shop {
    std::string stableId;
    std::string name;
    ShopType type = ShopType::General;
    std::vector<ShopItem> stock;
    int restockTimer = 0;
    bool infiniteGold = false;

    void generateStock(int depth, class Rng& rng);
    bool buy(size_t idx, int& playerGold, int& playerRubies, Item& out);
    bool sell(const Item& item, int& playerGold);
};

}
