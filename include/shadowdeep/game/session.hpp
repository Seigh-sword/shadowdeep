#pragma once
#include <array>
#include <deque>
#include <vector>
#include <string>
#include "../core/rng.hpp"
#include "../world/dungeon.hpp"
#include "../entities/player.hpp"
#include "../entities/monster.hpp"
#include "../items/item.hpp"
#include "../world/region.hpp"
#include "../terminal/color.hpp"

namespace shadowdeep {

constexpr int kMaxDepth = 30;
constexpr int kMsgLines = 6;

struct Floor {
    Dungeon dungeon;
    std::vector<Monster> monsters;
    std::vector<Item> items;
    bool initialized = false;
    int depth = 1;
    RegionId region = RegionId::ForgottenCellars;
};

struct GameMessage {
    std::string text;
    Color color = Color::Default;
    int count = 1;
};

class GameSession {
public:
    explicit GameSession(uint32_t seed = 0, bool hasSeed = false);

    void newGame(const std::string& entryId, const std::string& charName, int pclass, uint32_t seed, const std::string& difficulty);
    bool loadFromData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> saveToData() const;

    void start();
    bool isRunning() const;
    bool isDead() const;
    bool isWon() const;

    Player& player();
    const Player& player() const;
    Floor& currentFloor();
    const Floor& currentFloor() const;
    Dungeon& dungeon();
    const Dungeon& dungeon() const;
    std::vector<Monster>& monsters();
    const std::vector<Monster>& monsters() const;
    std::vector<Item>& items();
    const std::vector<Item>& items() const;

    int depth() const;
    Rng& rng();
    const Rng& rng() const;

    void ensureFloor(int d);
    void computeFov();

    bool movePlayer(int dx, int dy);
    bool pickup();
    bool descend();
    bool ascend();
    bool waitTurn();
    bool useItem(size_t idx);
    bool dropItem(size_t idx);
    bool quaffPotion();
    bool readScroll();
    void endTurn();

    void addMessage(const std::string& s, Color c = Color::Default);
    const std::deque<GameMessage>& messages() const;

    long long kills() const;
    long long goldEarned() const;
    long long startTimeMs() const;

    std::string elapsedString() const;

    void setEntryId(const std::string& id);
    std::string entryId() const;

private:
    Rng rng_;
    bool hasSeed_ = false;
    Player player_;
    std::array<Floor, kMaxDepth + 1> floors_;
    int depth_ = 1;
    bool running_ = false;
    bool dead_ = false;
    bool won_ = false;
    long long kills_ = 0;
    long long goldEarned_ = 0;
    long long startedMs_ = 0;
    std::deque<GameMessage> messages_;
    std::string entryId_;

    void resetFloors();
    Monster createMonster(const MonsterTemplate& t, Vec2 p);
    Item randomItem(Vec2 pos, int depth);
    Item makeHealPotion(Vec2 pos);
    Vec2 randomWalkable();
    bool monsterAt(Vec2 p) const;
    Monster* monsterAtPtr(Vec2 p);
    int itemIndexAt(Vec2 p) const;
    void playerAttack(Monster& m);
    void killMonster(Monster& m);
    int rollDamage();
    void monsterTurns();
    void updateMonster(Monster& m);
    void wander(Monster& m);
    bool monsterCanSee(const Monster& m) const;
    std::vector<Vec2> pathToPlayer(const Monster& src);
    void monsterAttack(Monster& m);
    void autoPickup();
};

}
