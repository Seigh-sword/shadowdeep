#include "shadowdeep/game/session.hpp"
#include "shadowdeep/world/generator.hpp"
#include "shadowdeep/world/region.hpp"
#include "shadowdeep/save/binary_writer.hpp"
#include "shadowdeep/save/binary_reader.hpp"
#include "shadowdeep/save/save_header.hpp"
#include "shadowdeep/version.hpp"
#include <chrono>
#include <queue>
#include <algorithm>
#include <limits>
#include <random>
#include <cstdio>

namespace shadowdeep {

static long long nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

GameSession::GameSession(uint32_t seed, bool hasSeed) : rng_(seed), hasSeed_(hasSeed) {
    if (!hasSeed) {
        rng_ = Rng();
    }
    resetFloors();
    startedMs_ = nowMs();
}

void GameSession::resetFloors() {
    floors_.clear();
    floors_.resize(kMaxDepth + 1);
    for (int i = 0; i <= kMaxDepth; ++i) {
        floors_[i] = Floor{};
        floors_[i].depth = i;
        floors_[i].region = regionForDepth(i);
        floors_[i].monsterPowerScale = monsterScaleForDepth(i);
    }
}

int GameSession::monsterScaleForDepth(int d) const {
    if (d <= kOriginalMaxDepth) return 0;
    int extra = d - kOriginalMaxDepth;
    return extra * 3 + (extra * extra) / 10;
}

void GameSession::newGame(const std::string& entryId, const std::string& charName, int pclass, uint32_t seed, const std::string& difficulty) {
    (void)difficulty;
    entryId_ = entryId;
    rng_.reseed(seed);
    hasSeed_ = true;
    player_ = Player{};
    player_.name = charName;
    player_.pclass = static_cast<PlayerClass>(pclass % 6);
    player_.entryId = entryId;
    player_.stats = PlayerStats{};
    player_.stats.maxHp = 30 + pclass * 2;
    player_.stats.hp = player_.stats.maxHp;
    player_.stats.baseAtk = 4 + (pclass == 0 ? 2 : 0);
    player_.stats.baseDef = (pclass == 0 ? 1 : 0);
    player_.stats.hunger = 1200;
    player_.stats.maxHunger = 2000;
    player_.stats.saturation = 600;
    player_.stats.maxSaturation = 2000;

    depth_ = 1;
    running_ = true;
    dead_ = false;
    won_ = false;
    kills_ = 0;
    goldEarned_ = 0;
    messages_.clear();
    startedMs_ = nowMs();
    codex_ = GuideCodex{};

    resetFloors();

    Item dagger;
    dagger.stableId = "item.dagger";
    dagger.name = "dagger";
    dagger.kind = ItemKind::Weapon;
    dagger.glyph = '/';
    dagger.unicodeGlyph = "⚔";
    dagger.color = Color::Steel;
    dagger.power = 2 + (pclass == 1 ? 1 : 0);
    dagger.slot = EquipSlot::MainHand;
    dagger.identified = true;

    std::optional<Item> prev;
    player_.equipment.equip(dagger, prev);

    for (int i = 0; i < 2; ++i) {
        Item heal = makeHealPotion({0,0});
        player_.inventory.add(heal);
    }

    if (pclass == 3) {
        Item scroll;
        scroll.stableId = "item.scroll_lightning";
        scroll.name = "scroll of lightning";
        scroll.kind = ItemKind::ScrollLightning;
        scroll.glyph = '?';
        scroll.unicodeGlyph = "📜";
        scroll.color = Color::BrightYellow;
        scroll.power = 18;
        player_.inventory.add(scroll);
    }

    Item ration;
    ration.stableId = "item.food";
    ration.name = "food ration";
    ration.kind = ItemKind::Food;
    ration.glyph = '%';
    ration.unicodeGlyph = "🍖";
    ration.color = Color::Brown;
    ration.power = 10;
    player_.inventory.add(ration);

    ensureFloor(1);
    player_.pos = floors_[1].dungeon.stairsUp;

    addMessage("Welcome to the Shadowdeep.", Color::BrightYellow);
    addMessage("Recover the Amulet and return to the surface.", Color::BrightYellow);
    addMessage("Hunger drains. Eat to stay sated and regen health.", Color::Brown);
    addMessage("Collect guide fragments to unlock knowledge.", Color::BrightCyan);
    computeFov();
}

bool GameSession::loadFromData(const std::vector<uint8_t>& data) {
    (void)data;
    return false;
}

std::vector<uint8_t> GameSession::saveToData() const {
    BinaryWriter w;
    w.writeU32(kSaveMagic);
    w.writeU16(kContainerFormatVersion);
    w.writeU16(kSaveSchemaVersion);
    for (int i = 0; i < 16; ++i) w.writeU8(0);
    uint64_t now = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
    w.writeU64(now);
    w.writeU64(now);
    w.writeU64(0);
    w.writeU8(0);
    w.writeU8(0); w.writeU8(0); w.writeU8(0);
    w.writeU32(0);
    w.writeU32(0);

    w.writeString(entryId_);
    w.writeString(player_.name);
    w.writeString(player_.className());
    w.writeI32(player_.stats.level);
    w.writeI32(depth_);
    w.writeString(regionNameForDepth(depth_));
    w.writeI64(player_.stats.playTimeMs + (nowMs() - startedMs_));
    w.writeString("normal");
    w.writeString(std::to_string(rng_.seedValue()));
    w.writeU8(dead_ ? 2 : (won_ ? 1 : 0));
    w.writeI32(player_.stats.gold);
    w.writeI32(player_.stats.rubies);
    w.writeI32(static_cast<int32_t>(kills_));
    w.writeBool(player_.hasAmulet);

    w.writeI32(player_.stats.hp);
    w.writeI32(player_.stats.maxHp);
    w.writeI32(player_.stats.level);
    w.writeI32(player_.stats.xp);
    w.writeI32(player_.stats.xpNext);
    w.writeI32(player_.pos.x);
    w.writeI32(player_.pos.y);
    w.writeI32(depth_);
    w.writeU32(rng_.seedValue());
    w.writeI32(player_.stats.hunger);
    w.writeI32(player_.stats.saturation);

    return w.data();
}

void GameSession::start() {
    running_ = true;
}

bool GameSession::isRunning() const { return running_; }
bool GameSession::isDead() const { return dead_; }
bool GameSession::isWon() const { return won_; }

Player& GameSession::player() { return player_; }
const Player& GameSession::player() const { return player_; }

Floor& GameSession::currentFloor() { return floors_[depth_]; }
const Floor& GameSession::currentFloor() const { return floors_[depth_]; }

Dungeon& GameSession::dungeon() { return floors_[depth_].dungeon; }
const Dungeon& GameSession::dungeon() const { return floors_[depth_].dungeon; }

std::vector<Monster>& GameSession::monsters() { return floors_[depth_].monsters; }
const std::vector<Monster>& GameSession::monsters() const { return floors_[depth_].monsters; }

std::vector<Item>& GameSession::items() { return floors_[depth_].items; }
const std::vector<Item>& GameSession::items() const { return floors_[depth_].items; }

int GameSession::depth() const { return depth_; }
Rng& GameSession::rng() { return rng_; }
const Rng& GameSession::rng() const { return rng_; }

GuideCodex& GameSession::codex() { return codex_; }
const GuideCodex& GameSession::codex() const { return codex_; }

Item GameSession::makeHealPotion(Vec2 pos) {
    Item i;
    i.stableId = "item.potion_heal";
    i.name = "potion of healing";
    i.kind = ItemKind::PotionHeal;
    i.glyph = '!';
    i.unicodeGlyph = "🧪";
    i.color = Color::BrightRed;
    i.power = 18;
    i.pos = pos;
    i.identified = true;
    return i;
}

Item GameSession::randomItem(Vec2 pos, int d) {
    Item i;
    i.pos = pos;
    i.identified = true;

    int r = rng_.range(1, 100);

    if (r <= 12) {
        i = makeHealPotion(pos);
    } else if (r <= 14 && d >= 3) {
        i.stableId = "item.potion_greater_heal";
        i.name = "potion of greater healing";
        i.kind = ItemKind::PotionGreaterHeal;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightRed;
        i.power = 35;
    } else if (r <= 16 && d >= 3) {
        i.stableId = "item.potion_strength";
        i.name = "potion of strength";
        i.kind = ItemKind::PotionStrength;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightMagenta;
        i.power = 1;
    } else if (r <= 18 && d >= 3) {
        i.stableId = "item.potion_haste";
        i.name = "potion of haste";
        i.kind = ItemKind::PotionHaste;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightCyan;
        i.power = 18;
    } else if (r <= 20 && d >= 6) {
        i.stableId = "item.potion_rejuvenation";
        i.name = "potion of rejuvenation";
        i.kind = ItemKind::PotionRejuvenation;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightYellow;
    } else if (r <= 22 && d >= 8) {
        i.stableId = "item.potion_invisibility";
        i.name = "potion of invisibility";
        i.kind = ItemKind::PotionInvisibility;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::Gray;
        i.power = 20;
    } else if (r <= 24) {
        i.stableId = "item.potion_mana";
        i.name = "potion of mana";
        i.kind = ItemKind::PotionMana;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightCyan;
        i.power = 15;
    } else if (r <= 26) {
        i.stableId = "item.potion_antidote";
        i.name = "potion of antidote";
        i.kind = ItemKind::PotionAntidote;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::Green;
    } else if (r <= 28 && d >= 10) {
        i.stableId = "item.potion_fire_resist";
        i.name = "potion of fire resistance";
        i.kind = ItemKind::PotionFireResist;
        i.glyph = '!';
        i.unicodeGlyph = "🧪";
        i.color = Color::BrightRed;
    } else if (r <= 35) {
        auto& all = allItemTemplates();
        int idx = rng_.range(0, 10);
        if (d > 10) idx = rng_.range(0, 25);
        if (d > 20) idx = rng_.range(10, 50);
        if (d > 30) idx = rng_.range(20, static_cast<int>(all.size()) - 30);
        idx = std::clamp(idx, 0, static_cast<int>(all.size()) - 1);
        auto& tmpl = all[idx];
        i.stableId = tmpl.stableId;
        i.name = tmpl.name;
        i.kind = tmpl.kind;
        i.glyph = tmpl.glyph;
        i.unicodeGlyph = tmpl.unicodeGlyph;
        i.color = tmpl.color;
        i.power = tmpl.basePower + rng_.range(0, 3) + (d > 30 ? (d - 30) / 5 : 0);
        i.slot = tmpl.slot;
        i.rarity = tmpl.rarity;
        if (rng_.chance(20 + d / 2)) {
            Enchantment ench;
            ench.id = static_cast<EnchantmentId>(rng_.range(1, 12));
            ench.power = rng_.range(1, 4);
            i.enchantments.push_back(ench);
            if (i.rarity < ItemRarity::Uncommon) i.rarity = ItemRarity::Uncommon;
        }
    } else if (r <= 42) {
        auto& all = allItemTemplates();
        std::vector<int> armorIdx;
        for (int j = 0; j < static_cast<int>(all.size()); ++j) {
            if (all[j].kind == ItemKind::Armor || all[j].kind == ItemKind::Shield || all[j].kind == ItemKind::Helmet || all[j].kind == ItemKind::Boots) armorIdx.push_back(j);
        }
        if (!armorIdx.empty()) {
            int pick = armorIdx[rng_.range(0, static_cast<int>(armorIdx.size()) - 1)];
            auto& tmpl = all[pick];
            i.stableId = tmpl.stableId;
            i.name = tmpl.name;
            i.kind = tmpl.kind;
            i.glyph = tmpl.glyph;
            i.unicodeGlyph = tmpl.unicodeGlyph;
            i.color = tmpl.color;
            i.power = tmpl.basePower + rng_.range(0, 2);
            i.slot = tmpl.slot;
            i.rarity = tmpl.rarity;
        }
    } else if (r <= 48) {
        i.stableId = "item.food";
        i.name = "food ration";
        i.kind = ItemKind::Food;
        i.glyph = '%';
        i.unicodeGlyph = "🍖";
        i.color = Color::Brown;
        i.power = 12;
    } else if (r <= 52) {
        const char* foods[] = {"bread", "meat", "iron ration", "fruit", "cheese", "jerky"};
        ItemKind kinds[] = {ItemKind::FoodBread, ItemKind::FoodMeat, ItemKind::FoodRation, ItemKind::FoodFruit, ItemKind::Food, ItemKind::FoodMeat};
        int f = rng_.range(0, 5);
        i.stableId = std::string("item.food_") + foods[f];
        i.name = foods[f];
        i.kind = kinds[f];
        i.glyph = '%';
        i.unicodeGlyph = "🍖";
        i.color = f == 3 ? Color::BrightGreen : (f == 1 ? Color::BrightRed : Color::Brown);
        i.power = 8 + rng_.range(0, 8);
    } else if (r <= 58) {
        i.stableId = "item.gold";
        i.name = "pile of gold";
        i.kind = ItemKind::Gold;
        i.glyph = '$';
        i.unicodeGlyph = "💰";
        i.color = Color::Gold;
        i.power = rng_.range(4, 8 + d * 4);
        i.valueGold = i.power;
    } else if (r <= 62) {
        i.stableId = "item.ruby";
        i.name = "ruby";
        i.kind = ItemKind::Ruby;
        i.glyph = '$';
        i.unicodeGlyph = "💎";
        i.color = Color::BrightRed;
        i.power = 1;
        i.valueGold = rng_.range(1, 4 + d / 5);
    } else if (r <= 66) {
        i.stableId = "item.scroll_lightning";
        i.name = "scroll of lightning";
        i.kind = ItemKind::ScrollLightning;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::BrightYellow;
        i.power = 18 + d * 2;
    } else if (r <= 70 && d >= 3) {
        i.stableId = "item.scroll_fireball";
        i.name = "scroll of fireball";
        i.kind = ItemKind::ScrollFireball;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::BrightRed;
        i.power = 30 + d * 2;
    } else if (r <= 74) {
        i.stableId = "item.scroll_teleport";
        i.name = "scroll of teleport";
        i.kind = ItemKind::ScrollTeleport;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::BrightMagenta;
    } else if (r <= 78) {
        i.stableId = "item.scroll_mapping";
        i.name = "scroll of magic mapping";
        i.kind = ItemKind::ScrollMapping;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::BrightCyan;
    } else if (r <= 82) {
        i.stableId = "item.scroll_identify";
        i.name = "scroll of identify";
        i.kind = ItemKind::ScrollIdentify;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::White;
    } else if (r <= 86 && d >= 10) {
        i.stableId = "item.scroll_enchant";
        i.name = "scroll of enchantment";
        i.kind = ItemKind::ScrollEnchant;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::Gold;
    } else if (r <= 88) {
        i.stableId = "item.bomb";
        i.name = "bomb";
        i.kind = ItemKind::Bomb;
        i.glyph = '*';
        i.unicodeGlyph = "💣";
        i.color = Color::BrightRed;
        i.power = 25 + d * 2;
    } else if (r <= 92) {
        i.stableId = "item.guide_fragment_common";
        i.name = "guide fragment";
        i.kind = ItemKind::GuideFragment;
        i.glyph = ';';
        i.unicodeGlyph = "📖";
        i.color = Color::White;
        i.power = 1;
    } else if (r <= 95 && d >= 8) {
        i.stableId = "item.guide_fragment_rare";
        i.name = "ancient guide fragment";
        i.kind = ItemKind::GuideFragment;
        i.glyph = ';';
        i.unicodeGlyph = "📖";
        i.color = Color::Gold;
        i.power = 2;
        i.rarity = ItemRarity::Rare;
    } else if (r <= 98) {
        i.stableId = "item.lore_scroll";
        i.name = "lore scroll";
        i.kind = ItemKind::LoreScroll;
        i.glyph = '?';
        i.unicodeGlyph = "📜";
        i.color = Color::BrightYellow;
    } else {
        i.stableId = "item.key";
        i.name = "iron key";
        i.kind = ItemKind::Key;
        i.glyph = '-';
        i.unicodeGlyph = "🔑";
        i.color = Color::Steel;
    }

    i.pos = pos;
    return i;
}

Vec2 GameSession::randomWalkable() {
    for (int n = 0; n < 3000; ++n) {
        Vec2 p{rng_.range(1, floors_[depth_].dungeon.mapW - 2), rng_.range(1, floors_[depth_].dungeon.mapH - 2)};
        if (dungeon().walkable(p) && !monsterAt(p) && p != dungeon().stairsUp && p != dungeon().stairsDown) return p;
    }
    return dungeon().stairsUp;
}

bool GameSession::monsterAt(Vec2 p) const {
    for (auto& m : floors_[depth_].monsters) if (m.alive && m.pos == p) return true;
    return false;
}

Monster* GameSession::monsterAtPtr(Vec2 p) {
    for (auto& m : monsters()) if (m.alive && m.pos == p) return &m;
    return nullptr;
}

int GameSession::itemIndexAt(Vec2 p) const {
    auto& v = floors_[depth_].items;
    for (int i = 0; i < static_cast<int>(v.size()); ++i) if (v[i].pos == p) return i;
    return -1;
}

Monster GameSession::createMonster(const MonsterTemplate& t, Vec2 p, int depth) {
    Monster m;
    m.stableId = t.stableId;
    m.name = t.name;
    m.glyph = t.glyph;
    m.unicodeGlyph = t.unicodeGlyph;
    m.color = t.color;
    m.pos = p;
    int scale = monsterScaleForDepth(depth);
    m.hp = t.baseHp + rng_.range(0, t.hpExtra) + scale;
    m.maxHp = m.hp;
    m.atk = t.baseAtk + scale / 4;
    m.def = t.baseDef + scale / 8;
    m.xp = t.xp + scale / 2;
    m.speed = t.speed;
    m.erratic = t.erratic;
    m.boss = t.stableId == "monster.ancient_dragon" || t.stableId.find("_king") != std::string::npos || t.stableId.find("_lord") != std::string::npos || t.stableId.find("titan") != std::string::npos || t.stableId.find("void_horror") != std::string::npos;
    m.canOpenDoors = t.canOpenDoors;
    m.canFly = t.canFly;
    m.family = t.family;
    m.damageType = t.damageType;
    m.id = rng_.range(1, 1000000);

    if (depth > 30) {
        double mult = 1.0 + (depth - 30) * 0.08;
        m.hp = static_cast<int>(m.hp * mult);
        m.maxHp = m.hp;
        m.atk = static_cast<int>(m.atk * mult);
        if (m.def < 20) m.def = static_cast<int>(m.def * (1.0 + (depth - 30) * 0.03));
    }
    return m;
}

void GameSession::populateShop(int depth) {
    Floor& f = floors_[depth];
    if (f.shopRoomIdx < 0 || f.shopRoomIdx >= static_cast<int>(f.dungeon.rooms.size())) return;
    auto& room = f.dungeon.rooms[f.shopRoomIdx];
    f.shopItems.clear();
    int shopCount = rng_.range(3, 6) + depth / 10;
    if (shopCount > 10) shopCount = 10;
    for (int i = 0; i < shopCount; ++i) {
        Vec2 p{room.x + 1 + rng_.range(0, room.w - 3), room.y + 1 + rng_.range(0, room.h - 3)};
        Item it = randomItem(p, depth + 2);
        it.valueGold = std::max(5, it.power * 5 + rng_.range(5, 20 + depth * 2));
        if (it.rarity == ItemRarity::Rare) it.valueGold += 30;
        if (it.rarity == ItemRarity::Epic) it.valueGold += 80;
        if (it.rarity == ItemRarity::Legendary) it.valueGold += 150;
        it.pos = p;
        f.shopItems.push_back(it);
    }
    Vec2 keeperPos = room.center();
    auto* keeperT = findMonsterTemplate("monster.shopkeeper");
    if (keeperT) {
        f.monsters.push_back(createMonster(*keeperT, keeperPos, depth));
        f.shopkeeperPos = keeperPos;
        f.shopkeeperAlive = true;
        auto* guardT = findMonsterTemplate("monster.shop_guard");
        if (guardT && rng_.chance(60)) {
            Vec2 gp = keeperPos + Vec2{rng_.range(-1,1), rng_.range(-1,1)};
            if (f.dungeon.walkable(gp)) {
                f.monsters.push_back(createMonster(*guardT, gp, depth));
            }
        }
    }
}

void GameSession::trySpawnMimic(Vec2 pos) {
    auto* mimicT = findMonsterTemplate(rng_.chance(50) ? "monster.mimic" : "monster.chest_mimic");
    if (!mimicT) return;
    Floor& f = floors_[depth_];
    if (monsterAt(pos)) return;
    f.monsters.push_back(createMonster(*mimicT, pos, depth_));
    f.mimicCount++;
}

void GameSession::ensureFloor(int d) {
    if (d < 1 || d > kMaxDepth) return;
    Floor& f = floors_[d];
    if (f.initialized) return;

    DungeonGenerator gen(rng_);
    gen.generate(f.dungeon, d);
    f.initialized = true;
    f.depth = d;
    f.region = regionForDepth(d);
    f.biome = f.dungeon.biome;
    f.monsterPowerScale = monsterScaleForDepth(d);
    f.randomEventCooldown = rng_.range(50, 150);

    int count = 8 + d / 2 + rng_.range(0, 5);
    if (d >= 20) count += 4;
    if (d > 30) count += (d - 30) / 3;
    if (count > 50) count = 50;

    std::vector<const MonsterTemplate*> eligible;
    for (auto& t : allMonsterTemplates()) {
        if (t.stableId == "monster.ancient_dragon" && d != kOriginalMaxDepth) continue;
        if (t.stableId == "monster.shopkeeper" || t.stableId == "monster.shop_guard") continue;
        if (t.stableId == "monster.mimic" || t.stableId == "monster.chest_mimic") continue;
        if (d >= t.minDepth && d <= t.maxDepth) eligible.push_back(&t);
        else if (d > 30 && t.maxDepth >= 25) eligible.push_back(&t);
    }

    if (d > 40) {
        for (auto& t : allMonsterTemplates()) {
            if (t.stableId.find("shop") != std::string::npos) continue;
            if (t.stableId.find("mimic") != std::string::npos) continue;
            if (t.minDepth >= 20) eligible.push_back(&t);
        }
    }

    for (int n = 0; n < count; ++n) {
        if (eligible.empty()) break;
        Vec2 p;
        for (int tries = 0; tries < 800; ++tries) {
            p = {rng_.range(1, f.dungeon.mapW - 2), rng_.range(1, f.dungeon.mapH - 2)};
            bool occupied = false;
            for (auto& m : f.monsters) if (m.alive && m.pos == p) occupied = true;
            if (f.dungeon.walkable(p) && !occupied && p.manhattan(f.dungeon.stairsUp) >= 6) break;
        }
        const MonsterTemplate* t = eligible[rng_.range(0, static_cast<int>(eligible.size()) - 1)];
        f.monsters.push_back(createMonster(*t, p, d));
    }

    if (d > 10 && rng_.chance(30)) {
        std::vector<std::string> bosses = {"monster.goblin_king", "monster.orc_warlord", "monster.lich_king", "monster.demon_lord", "monster.shadow_lord"};
        if (d > 30) bosses.insert(bosses.end(), {"monster.void_horror", "monster.titan", "monster.dragon_elder"});
        std::string bossId = bosses[rng_.range(0, static_cast<int>(bosses.size()) - 1)];
        auto* bossT = findMonsterTemplate(bossId);
        if (bossT) {
            Vec2 bp = f.dungeon.rooms.empty() ? f.dungeon.stairsDown : f.dungeon.rooms[rng_.range(0, static_cast<int>(f.dungeon.rooms.size()) - 1)].center();
            f.monsters.push_back(createMonster(*bossT, bp, d));
        }
    }

    int itemCount = 6 + rng_.range(0, 6) + d / 3;
    if (d > 30) itemCount += 3;
    for (int n = 0; n < itemCount; ++n) {
        Vec2 p;
        for (int tries = 0; tries < 800; ++tries) {
            p = {rng_.range(1, f.dungeon.mapW - 2), rng_.range(1, f.dungeon.mapH - 2)};
            bool used = false;
            for (auto& it : f.items) if (it.pos == p) used = true;
            if (f.dungeon.walkable(p) && p != f.dungeon.stairsUp && p != f.dungeon.stairsDown && !used) break;
        }
        f.items.push_back(randomItem(p, d));
    }

    if (rng_.chance(40)) {
        Vec2 p;
        for (int tries = 0; tries < 500; ++tries) {
            p = {rng_.range(1, f.dungeon.mapW - 2), rng_.range(1, f.dungeon.mapH - 2)};
            if (f.dungeon.walkable(p) && p != f.dungeon.stairsUp && p != f.dungeon.stairsDown) break;
        }
        Item frag;
        frag.stableId = "item.guide_fragment_common";
        frag.name = "guide fragment";
        frag.kind = ItemKind::GuideFragment;
        frag.glyph = ';';
        frag.unicodeGlyph = "📖";
        frag.color = Color::White;
        frag.pos = p;
        frag.identified = true;
        f.items.push_back(frag);
    }

    if (d >= 2 && rng_.chance(35) && f.dungeon.rooms.size() >= 4) {
        int idx = rng_.range(1, static_cast<int>(f.dungeon.rooms.size()) - 2);
        f.hasShop = true;
        f.shopRoomIdx = idx;
        auto& room = f.dungeon.rooms[idx];
        for (int y = room.y; y < room.y + room.h; ++y) {
            for (int x = room.x; x < room.x + room.w; ++x) {
                Vec2 p{x,y};
                if (f.dungeon.inBounds(p) && f.dungeon.at(p) == Tile::Wall) {
                    if (x == room.x || x == room.x + room.w - 1 || y == room.y || y == room.y + room.h - 1) continue;
                    f.dungeon.set(p, Tile::Floor);
                }
            }
        }
        floors_[d] = f;
        populateShop(d);
    }

    int chestCount = 0;
    for (int y = 1; y < f.dungeon.mapH - 1; ++y) {
        for (int x = 1; x < f.dungeon.mapW - 1; ++x) {
            if (f.dungeon.tiles[y][x] == Tile::Chest) {
                chestCount++;
                if (rng_.chance(25)) {
                    f.dungeon.tiles[y][x] = Tile::Floor;
                    Vec2 pos{x,y};
                    auto* mimicT = findMonsterTemplate(rng_.chance(60) ? "monster.chest_mimic" : "monster.mimic");
                    if (mimicT) {
                        bool occupied = false;
                        for (auto& m : f.monsters) if (m.alive && m.pos == pos) occupied = true;
                        if (!occupied) {
                            f.monsters.push_back(createMonster(*mimicT, pos, d));
                            f.mimicCount++;
                        }
                    }
                }
            }
        }
    }

    if (d == kOriginalMaxDepth) {
        Vec2 bossPos = f.dungeon.rooms.empty() ? f.dungeon.stairsDown : f.dungeon.rooms.back().center();
        if (bossPos == f.dungeon.stairsDown) bossPos = bossPos + Vec2{-1, 0};
        if (!f.dungeon.walkable(bossPos)) bossPos = f.dungeon.rooms.back().center();

        auto* dragonT = findMonsterTemplate("monster.ancient_dragon");
        if (dragonT) f.monsters.push_back(createMonster(*dragonT, bossPos, d));

        Vec2 amuletPos = f.dungeon.stairsDown;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                Vec2 q = amuletPos + Vec2{dx, dy};
                bool occupied = false;
                for (auto& m : f.monsters) if (m.alive && m.pos == q) occupied = true;
                if (f.dungeon.walkable(q) && !occupied) {
                    amuletPos = q;
                    dx = 2; break;
                }
            }
        }

        Item a;
        a.stableId = "item.amulet_shadowdeep";
        a.name = "Amulet of Shadowdeep";
        a.kind = ItemKind::AmuletShadowdeep;
        a.glyph = '"';
        a.unicodeGlyph = "🔮";
        a.color = Color::Gold;
        a.pos = amuletPos;
        a.rarity = ItemRarity::Mythic;
        a.identified = true;
        f.items.push_back(a);
    }
}

void GameSession::computeFov() {
    dungeon().computeFov(player_.pos, kFovRadius);
    dungeon().updateCamera(player_.pos);
}

bool GameSession::movePlayer(int dx, int dy) {
    Vec2 target = player_.pos + Vec2{dx, dy};
    if (!dungeon().inBounds(target)) return false;

    Monster* m = monsterAtPtr(target);
    if (m) {
        if (m->stableId == "monster.shopkeeper") {
            addMessage("The shopkeeper eyes you warily. Press 'b' to browse.", Color::BrightYellow);
            return false;
        }
        playerAttack(*m);
        return true;
    }

    if (dungeon().blocksMove(target)) return false;

    if (dungeon().at(target) == Tile::DoorClosed) {
        dungeon().set(target, Tile::DoorOpen);
        addMessage("You open the door.", Color::Brown);
        return true;
    }

    player_.pos = target;
    autoPickup();
    handleTileEffects();
    revealMimicsNear(target);
    return true;
}

void GameSession::revealMimicsNear(Vec2 pos) {
    for (auto& mon : monsters()) {
        if (!mon.alive) continue;
        if (mon.stableId.find("mimic") == std::string::npos) continue;
        if (mon.pos.chebyshev(pos) <= 1) {
            if (!mon.aware) {
                mon.aware = true;
                mon.aiState = AiState::Hunting;
                addMessage(std::string("The ") + mon.name + " reveals itself! It was a mimic!", Color::BrightRed);
            }
        }
    }
}

void GameSession::handleTileEffects() {
    Tile t = dungeon().at(player_.pos);
    Floor& f = currentFloor();
    if (t == Tile::Altar && !f.altarUsed) {
        handleAltar();
    } else if (t == Tile::Fountain && !f.fountainDry) {
        handleFountain();
    } else if (t == Tile::Chest) {
        handleChestInteraction();
    } else if (t == Tile::Trap) {
        int dmg = rng_.range(5, 15 + depth_ * 2);
        player_.stats.hp -= dmg;
        addMessage(std::string("You trigger a trap for ") + std::to_string(dmg) + " damage!", Color::BrightRed);
        dungeon().set(player_.pos, Tile::Floor);
        if (rng_.chance(30)) {
            player_.effects.add(EffectId::Bleeding, 5, 2, "trap");
        }
    } else if (t == Tile::Lava) {
        int dmg = rng_.range(3, 8);
        player_.stats.hp -= dmg;
        addMessage(std::string("Lava sears you for ") + std::to_string(dmg) + " damage!", Color::BrightRed);
    }
}

void GameSession::handleAltar() {
    Floor& f = currentFloor();
    if (f.altarUsed) {
        addMessage("The altar is spent.", Color::Gray);
        return;
    }
    addMessage("You kneel before a shadowed altar.", Color::Purple);
    int roll = rng_.range(1, 100);
    if (roll <= 25 && player_.stats.gold >= 20) {
        player_.stats.gold -= 20;
        player_.stats.maxHp += 5;
        player_.stats.hp = player_.stats.maxHp;
        addMessage("You offer gold. The altar blesses you: +5 max HP and full heal!", Color::BrightYellow);
        codex_.unlockGuide("guide.altar_blessing");
        f.altarUsed = true;
        f.altarBlessing = 1;
    } else if (roll <= 50) {
        player_.stats.strength += 1;
        addMessage("Shadow energy flows into you: +1 strength!", Color::BrightMagenta);
        f.altarUsed = true;
        f.altarBlessing = 2;
    } else if (roll <= 70) {
        Item frag;
        frag.stableId = "item.guide_fragment_rare";
        frag.name = "altar fragment";
        frag.kind = ItemKind::GuideFragment;
        frag.glyph = ';';
        frag.unicodeGlyph = "📖";
        frag.color = Color::Gold;
        frag.pos = player_.pos;
        frag.identified = true;
        items().push_back(frag);
        addMessage("The altar reveals a guide fragment!", Color::Gold);
        f.altarUsed = true;
    } else if (roll <= 85) {
        player_.effects.add(EffectId::Cursed, 20, 1, "altar");
        addMessage("The altar curses you! You feel weakened.", Color::BrightRed);
        f.altarUsed = true;
        f.altarBlessing = -1;
    } else {
        addMessage("The altar hums but nothing happens. It awaits a worthier offering.", Color::Gray);
    }
}

void GameSession::handleFountain() {
    Floor& f = currentFloor();
    if (f.fountainDry) {
        addMessage("The fountain is dry.", Color::Gray);
        return;
    }
    addMessage("You find a shimmering fountain.", Color::BrightCyan);
    int roll = rng_.range(1, 100);
    if (roll <= 35) {
        int heal = rng_.range(10, 30 + depth_ * 2);
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + heal);
        addMessage(std::string("You drink and recover ") + std::to_string(heal) + " HP!", Color::BrightGreen);
        if (rng_.chance(20)) {
            f.fountainDry = true;
            addMessage("The fountain dries up.", Color::Gray);
        }
    } else if (roll <= 55) {
        player_.stats.hunger = player_.stats.maxHunger;
        player_.stats.saturation = player_.stats.maxSaturation;
        addMessage("Refreshing water! Hunger and saturation fully restored!", Color::BrightCyan);
        f.fountainDry = rng_.chance(30);
    } else if (roll <= 70) {
        player_.effects.add(EffectId::Poison, 8, 2, "fountain");
        addMessage("The water was tainted! You are poisoned!", Color::Green);
        f.fountainDry = true;
    } else if (roll <= 85) {
        player_.stats.maxHp += 3;
        player_.stats.hp += 3;
        addMessage("The fountain grants vitality: +3 max HP!", Color::BrightYellow);
        f.fountainDry = true;
    } else {
        Item it = randomItem(player_.pos, depth_ + 1);
        it.pos = player_.pos;
        items().push_back(it);
        addMessage(std::string("Something glints in the fountain: ") + it.name + "!", Color::Gold);
        f.fountainDry = rng_.chance(50);
    }
    codex_.addFragment("guide.fountain", "fountain");
}

void GameSession::handleChestInteraction() {
    Tile t = dungeon().at(player_.pos);
    if (t != Tile::Chest) return;
    addMessage("You open a chest!", Color::Gold);
    dungeon().set(player_.pos, Tile::Floor);
    int roll = rng_.range(1, 100);
    if (roll <= 20 && rng_.chance(30)) {
        trySpawnMimic(player_.pos);
        addMessage("It was a mimic!", Color::BrightRed);
        return;
    }
    for (int i = 0; i < rng_.range(1, 3); ++i) {
        Item it = randomItem(player_.pos, depth_ + 1);
        it.pos = player_.pos;
        items().push_back(it);
        addMessage(std::string("Chest contains: ") + it.name, it.color);
    }
    if (rng_.chance(30)) {
        int g = rng_.range(10, 30 + depth_ * 5);
        Item gold;
        gold.stableId = "item.gold";
        gold.name = "pile of gold";
        gold.kind = ItemKind::Gold;
        gold.glyph = '$';
        gold.unicodeGlyph = "💰";
        gold.color = Color::Gold;
        gold.power = g;
        gold.valueGold = g;
        gold.pos = player_.pos;
        items().push_back(gold);
    }
}

void GameSession::triggerRandomEvent() {
    Floor& f = currentFloor();
    if (f.randomEventCooldown > 0) {
        f.randomEventCooldown--;
        return;
    }
    if (!rng_.chance(5)) return;
    int ev = rng_.range(1, 10);
    switch (ev) {
        case 1: {
            addMessage("You hear distant scratching... a swarm approaches!", Color::BrightRed);
            for (int i = 0; i < rng_.range(2, 5); ++i) {
                Vec2 p = randomWalkable();
                auto* t = findMonsterTemplate("monster.giant_rat");
                if (t) f.monsters.push_back(createMonster(*t, p, depth_));
            }
            break;
        }
        case 2: {
            addMessage("A hidden cache is revealed!", Color::Gold);
            Vec2 p = randomWalkable();
            Item it = randomItem(p, depth_ + 3);
            it.rarity = ItemRarity::Rare;
            items().push_back(it);
            break;
        }
        case 3: {
            addMessage("You feel a cold wind from the void...", Color::Purple);
            if (depth_ > 20) player_.stats.hunger -= 50;
            break;
        }
        case 4: {
            addMessage("Your guide fragment glows faintly.", Color::BrightCyan);
            codex_.addFragment("guide.random_event", "random event");
            break;
        }
        case 5: {
            if (rng_.chance(30)) {
                int g = rng_.range(5, 15);
                player_.stats.gold += g;
                goldEarned_ += g;
                addMessage(std::string("You find ") + std::to_string(g) + " gold on the ground!", Color::Gold);
            }
            break;
        }
        case 6: {
            addMessage("The shadows shift...", Color::Gray);
            computeFov();
            break;
        }
        case 7: {
            if (player_.stats.hungerState() == HungerState::Full) {
                addMessage("You feel overfull and lose some saturation.", Color::Brown);
                player_.stats.saturation -= 100;
            }
            break;
        }
        case 8: {
            addMessage("A distant roar echoes through the dungeon!", Color::BrightRed);
            for (auto& m : monsters()) if (m.alive) m.aware = true;
            break;
        }
        case 9: {
            addMessage("You sense treasure nearby.", Color::Gold);
            for (int y = 0; y < dungeon().mapH; ++y) for (int x = 0; x < dungeon().mapW; ++x) dungeon().explored[y][x] = dungeon().explored[y][x] || rng_.chance(5);
            break;
        }
        default: break;
    }
    f.randomEventCooldown = rng_.range(80, 200);
}

bool GameSession::isInShop() const {
    const Floor& f = floors_[depth_];
    if (!f.hasShop) return false;
    if (f.shopRoomIdx < 0 || f.shopRoomIdx >= static_cast<int>(f.dungeon.rooms.size())) return false;
    auto& room = f.dungeon.rooms[f.shopRoomIdx];
    return room.contains(player_.pos);
}

const std::vector<Item>& GameSession::shopItems() const {
    return floors_[depth_].shopItems;
}

int GameSession::shopRoomIndex() const {
    return floors_[depth_].shopRoomIdx;
}

bool GameSession::buyShopItem(size_t idx) {
    return buyItemFromShop(idx);
}

bool GameSession::buyItemFromShop(size_t idx) {
    Floor& f = floors_[depth_];
    if (!isInShop()) {
        addMessage("You are not in a shop.", Color::Gray);
        return false;
    }
    if (idx >= f.shopItems.size()) {
        addMessage("No such item.", Color::Gray);
        return false;
    }
    Item it = f.shopItems[idx];
    if (player_.stats.gold < it.valueGold) {
        addMessage(std::string("Not enough gold. Need ") + std::to_string(it.valueGold) + ".", Color::BrightRed);
        return false;
    }
    if (!player_.inventory.hasSpace()) {
        addMessage("Inventory full.", Color::BrightRed);
        return false;
    }
    player_.stats.gold -= it.valueGold;
    it.pos = {0,0};
    player_.inventory.add(it);
    addMessage(std::string("You buy ") + it.name + " for " + std::to_string(it.valueGold) + " gold.", Color::Gold);
    f.shopItems.erase(f.shopItems.begin() + idx);
    return true;
}

void GameSession::useAltar() {
    Tile t = dungeon().at(player_.pos);
    if (t != Tile::Altar) {
        for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
            Vec2 q = player_.pos + Vec2{dx, dy};
            if (dungeon().inBounds(q) && dungeon().at(q) == Tile::Altar) {
                player_.pos = q;
                handleAltar();
                return;
            }
        }
        addMessage("No altar nearby.", Color::Gray);
        return;
    }
    handleAltar();
}

void GameSession::drinkFountain() {
    Tile t = dungeon().at(player_.pos);
    if (t != Tile::Fountain) {
        for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
            Vec2 q = player_.pos + Vec2{dx, dy};
            if (dungeon().inBounds(q) && dungeon().at(q) == Tile::Fountain) {
                player_.pos = q;
                handleFountain();
                return;
            }
        }
        addMessage("No fountain nearby.", Color::Gray);
        return;
    }
    handleFountain();
}

std::string GameSession::minimapString(int w, int h) const {
    const auto& d = floors_[depth_].dungeon;
    std::string out;
    int stepX = std::max(1, d.mapW / w);
    int stepY = std::max(1, d.mapH / h);
    for (int my = 0; my < h; ++my) {
        for (int mx = 0; mx < w; ++mx) {
            int wx = mx * stepX;
            int wy = my * stepY;
            if (wx >= d.mapW) wx = d.mapW - 1;
            if (wy >= d.mapH) wy = d.mapH - 1;
            if (wx == player_.pos.x && wy == player_.pos.y) { out += '@'; continue; }
            if (!d.explored[wy][wx]) { out += ' '; continue; }
            Tile t = d.tiles[wy][wx];
            char c = ' ';
            switch (t) {
                case Tile::Wall: c = '#'; break;
                case Tile::Floor: case Tile::Corridor: c = '.'; break;
                case Tile::DoorClosed: c = '+'; break;
                case Tile::DoorOpen: c = '/'; break;
                case Tile::StairsDown: c = '>'; break;
                case Tile::StairsUp: c = '<'; break;
                case Tile::Chest: c = 'C'; break;
                case Tile::Altar: c = 'A'; break;
                case Tile::Fountain: c = 'F'; break;
                case Tile::Trap: c = '^'; break;
                default: c = '.'; break;
            }
            out += c;
        }
        out += '\n';
    }
    return out;
}


void GameSession::autoPickup() {
    for (int i = 0; i < static_cast<int>(items().size()); ++i) {
        if (items()[i].pos == player_.pos) {
            if (items()[i].kind == ItemKind::Gold) {
                int g = items()[i].valueGold;
                player_.stats.gold += g;
                goldEarned_ += g;
                addMessage(std::string("You pick up ") + std::to_string(g) + " gold.", Color::Gold);
                items().erase(items().begin() + i);
                return;
            } else if (items()[i].kind == ItemKind::Ruby) {
                int r = items()[i].valueGold;
                player_.stats.rubies += r;
                addMessage(std::string("You pick up ") + std::to_string(r) + " ruby.", Color::BrightRed);
                items().erase(items().begin() + i);
                return;
            }
        }
    }
}

void GameSession::handleGuideFragment(const Item& it) {
    codex_.addFragment(it.stableId, it.name);
    addMessage(std::string("Guide fragment collected: ") + it.name + "!", Color::BrightCyan);
    if (codex_.unlockedCount() > 0) {
        addMessage(std::string("Codex now has ") + std::to_string(codex_.unlockedCount()) + "/" + std::to_string(codex_.totalGuides()) + " guides.", Color::BrightWhite);
    }
}

void GameSession::handleLoreScroll(const Item& it) {
    LoreFragment lf;
    lf.id = "lore.found_" + std::to_string(rng_.range(1000, 9999));
    lf.title = "Lore: " + it.name;
    lf.text = "Ancient knowledge from depth " + std::to_string(depth_) + ". The shadows whisper secrets of " + regionNameForDepth(depth_) + ".";
    lf.depthFound = depth_;
    lf.isBossDrop = false;
    codex_.addLore(lf);
    addMessage(std::string("Lore discovered: ") + lf.title, Color::Gold);
}

bool GameSession::pickup() {
    int idx = itemIndexAt(player_.pos);
    if (idx < 0) {
        addMessage("There is nothing here to pick up.", Color::Gray);
        return false;
    }

    Item it = items()[idx];

    if (it.kind == ItemKind::Gold) {
        player_.stats.gold += it.valueGold;
        goldEarned_ += it.valueGold;
        items().erase(items().begin() + idx);
        addMessage(std::string("You collect ") + std::to_string(it.valueGold) + " gold.", Color::Gold);
        return true;
    }

    if (it.kind == ItemKind::Ruby) {
        player_.stats.rubies += it.valueGold;
        items().erase(items().begin() + idx);
        addMessage("You collect a ruby.", Color::BrightRed);
        return true;
    }

    if (!player_.inventory.hasSpace()) {
        addMessage("Your pack is full.", Color::BrightRed);
        return false;
    }

    if (it.kind == ItemKind::AmuletShadowdeep) {
        player_.hasAmulet = true;
        addMessage("You seize the Amulet of Shadowdeep!", Color::BrightYellow);
        addMessage("Return to the surface. Or descend deeper for glory.", Color::Gold);
        codex_.unlockGuide("guide.amulet");
        codex_.addLore("lore.amulet_obtained", "Amulet Obtained", "You have claimed the Amulet of Shadowdeep. The dungeon trembles. The way back is open, but whispers call you deeper into the void.", depth_, false, "");
    } else if (it.kind == ItemKind::GuideFragment) {
        handleGuideFragment(it);
    } else if (it.kind == ItemKind::LoreScroll) {
        handleLoreScroll(it);
    } else {
        addMessage(std::string("You pick up ") + it.name + ".", it.color);
    }

    player_.inventory.add(it);
    items().erase(items().begin() + idx);
    return true;
}

bool GameSession::descend() {
    if (dungeon().at(player_.pos) != Tile::StairsDown) {
        addMessage("There are no downward stairs here.", Color::Gray);
        return false;
    }
    if (depth_ >= kMaxDepth) {
        addMessage("The dungeon goes no deeper... yet the void hungers.", Color::Purple);
        return false;
    }
    ++depth_;
    ensureFloor(depth_);
    player_.pos = dungeon().stairsUp;
    computeFov();
    if (depth_ > kOriginalMaxDepth) {
        addMessage(std::string("You descend to depth ") + std::to_string(depth_) + " (" + regionNameForDepth(depth_) + ") - Infinite depths!", Color::Purple);
        addMessage(std::string("Monsters grow stronger (+") + std::to_string(monsterScaleForDepth(depth_)) + " power).", Color::BrightRed);
    } else {
        addMessage(std::string("You descend to level ") + std::to_string(depth_) + std::string(" (") + regionNameForDepth(depth_) + ") [" + dungeon().biomeName() + "].", Color::BrightYellow);
    }
    if (depth_ == kOriginalMaxDepth) addMessage("A tremendous heat fills the darkness. The Amulet is near.", Color::BrightRed);
    if (depth_ == kOriginalMaxDepth + 1) {
        addMessage("You have passed the threshold. The true Shadowdeep begins.", Color::BrightMagenta);
        codex_.unlockGuide("guide.infinite_depth");
    }
    return false;
}

bool GameSession::ascend() {
    if (depth_ == 1) {
        if (player_.hasAmulet && player_.pos == dungeon().stairsUp) {
            won_ = true;
            return false;
        }
        if (player_.hasAmulet) addMessage("Find the way back to the surface.", Color::BrightYellow);
        else addMessage("You cannot leave without the Amulet.", Color::Gray);
        return false;
    }

    if (dungeon().at(player_.pos) != Tile::StairsUp) {
        addMessage("There are no upward stairs here.", Color::Gray);
        return false;
    }

    --depth_;
    player_.pos = dungeon().stairsDown;
    computeFov();
    addMessage(std::string("You ascend to level ") + std::to_string(depth_) + ".", Color::BrightCyan);
    return false;
}

int GameSession::rollDamage() {
    int base = player_.attackPower();
    int dmg = rng_.range(std::max(1, base - 1), base + 2);
    if (rng_.chance(15)) {
        dmg *= 2;
        addMessage("Critical hit!", Color::BrightYellow);
    }
    return dmg;
}

void GameSession::playerAttack(Monster& m) {
    int damage = std::max(1, rollDamage() - m.def);
    m.hp -= damage;
    m.aware = true;
    m.aiState = AiState::Hunting;

    addMessage(std::string("You hit the ") + m.name + std::string(" for ") + std::to_string(damage) + " damage.", Color::White);

    if (m.hp <= 0) killMonster(m);
}

void GameSession::killMonster(Monster& m) {
    m.alive = false;
    ++kills_;
    addMessage(std::string("The ") + m.name + " dies.", Color::BrightGreen);
    bool level = player_.gainXp(m.xp);
    addMessage(std::string("You gain ") + std::to_string(m.xp) + " XP.", Color::Gray);
    if (level) addMessage(std::string("You advance to level ") + std::to_string(player_.stats.level) + "!", Color::BrightYellow);

    if (m.boss) {
        addMessage(std::string("Boss defeated: ") + m.name + "!", Color::Gold);
        Item frag;
        frag.stableId = "item.guide_fragment_epic";
        frag.name = "forbidden guide fragment";
        frag.kind = ItemKind::GuideFragment;
        frag.glyph = ';';
        frag.unicodeGlyph = "📖";
        frag.color = Color::Purple;
        frag.pos = m.pos;
        frag.rarity = ItemRarity::Epic;
        frag.identified = true;
        items().push_back(frag);

        Item lore;
        lore.stableId = "item.lore_codex";
        lore.name = "codex of " + m.name;
        lore.kind = ItemKind::LoreScroll;
        lore.glyph = '?';
        lore.unicodeGlyph = "📜";
        lore.color = Color::Gold;
        lore.pos = m.pos;
        lore.identified = true;
        items().push_back(lore);

        LoreFragment lf;
        lf.id = "lore.boss_" + m.stableId;
        lf.title = "Defeated: " + m.name;
        lf.text = "You have slain " + m.name + " at depth " + std::to_string(depth_) + ". Its essence lingers, granting knowledge of the deep.";
        lf.depthFound = depth_;
        lf.isBossDrop = true;
        lf.bossSource = m.name;
        codex_.addLore(lf);

        if (rng_.chance(60)) {
            Item rare = randomItem(m.pos, depth_ + 5);
            rare.rarity = ItemRarity::Rare;
            if (rng_.chance(30)) rare.rarity = ItemRarity::Epic;
            if (rng_.chance(10)) rare.rarity = ItemRarity::Legendary;
            items().push_back(rare);
        }
    } else if (rng_.chance(22)) {
        Item drop = randomItem(m.pos, depth_);
        items().push_back(drop);
    } else if (rng_.chance(28)) {
        Item gold;
        gold.stableId = "item.gold";
        gold.name = "pile of gold";
        gold.kind = ItemKind::Gold;
        gold.glyph = '$';
        gold.unicodeGlyph = "💰";
        gold.color = Color::Gold;
        gold.power = rng_.range(2, 6 + depth_ * 2);
        gold.valueGold = gold.power;
        gold.pos = m.pos;
        items().push_back(gold);
    }

    if (m.boss) addMessage("The guardian has fallen! Loot and lore remain.", Color::BrightYellow);
}

void GameSession::endTurn() {
    ++player_.stats.turns;
    player_.tickHunger();
    computeFov();

    bool skipMonsterPhase = false;
    if (player_.hasteTurns > 0) {
        --player_.hasteTurns;
        skipMonsterPhase = (player_.stats.turns % 2 == 0);
    }
    if (player_.effects.has(EffectId::Haste)) skipMonsterPhase = (player_.stats.turns % 2 == 0);

    player_.effects.tick();

    if (player_.effects.has(EffectId::Poison)) {
        int dmg = player_.effects.getPower(EffectId::Poison);
        if (dmg < 1) dmg = 1;
        player_.stats.hp -= dmg;
        addMessage(std::string("You suffer ") + std::to_string(dmg) + " poison damage.", Color::Green);
    }

    if (player_.effects.has(EffectId::Burning)) {
        int dmg = player_.effects.getPower(EffectId::Burning);
        if (dmg < 1) dmg = 1;
        player_.stats.hp -= dmg;
        addMessage(std::string("You burn for ") + std::to_string(dmg) + " damage.", Color::BrightRed);
    }

    if (player_.effects.has(EffectId::Regeneration)) {
        int heal = player_.effects.getPower(EffectId::Regeneration);
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + heal);
    }

    if (player_.stats.isStarving()) {
        addMessage("You are starving!", Color::BrightRed);
    } else if (player_.stats.isHungry()) {
        if (player_.stats.turns % 50 == 0) addMessage("You feel hungry.", Color::Yellow);
    }

    if (player_.canRegenFromSaturation() && player_.stats.turns % 10 == 0) {
        addMessage("Your saturation heals you.", Color::BrightGreen);
    }

    if (player_.stats.turns % 30 == 0) {
        triggerRandomEvent();
    }

    if (!skipMonsterPhase) monsterTurns();

    if (player_.stats.hp <= 0) {
        dead_ = true;
        player_.stats.hp = 0;
    }

    auto& f = currentFloor();
    if (f.hasShop && f.shopkeeperAlive) {
        bool keeperAlive = false;
        for (auto& m : f.monsters) if (m.alive && m.stableId == "monster.shopkeeper") keeperAlive = true;
        if (!keeperAlive) {
            f.shopkeeperAlive = false;
            addMessage("The shopkeeper has been slain! The shop is now free loot, but guards are angry!", Color::BrightRed);
            for (auto& m : f.monsters) if (m.alive && m.stableId == "monster.shop_guard") { m.aware = true; m.aiState = AiState::Hunting; }
        }
    }
}

bool GameSession::monsterCanSee(const Monster& m) const {
    const Dungeon& d = floors_[depth_].dungeon;
    if (m.pos.chebyshev(player_.pos) > 16) return false;
    return d.lineVisible(m.pos, player_.pos);
}

std::vector<Vec2> GameSession::pathToPlayer(const Monster& source) {
    const int W = floors_[depth_].dungeon.mapW;
    const int H = floors_[depth_].dungeon.mapH;
    std::vector<std::vector<int>> dist(H, std::vector<int>(W, -1));
    std::vector<std::vector<Vec2>> parent(H, std::vector<Vec2>(W, {-1,-1}));
    std::queue<Vec2> q;
    q.push(source.pos);
    dist[source.pos.y][source.pos.x] = 0;

    while (!q.empty()) {
        Vec2 cur = q.front(); q.pop();
        if (cur == player_.pos) break;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                if (std::abs(dx) + std::abs(dy) == 2) continue;
                Vec2 next = cur + Vec2{dx, dy};
                if (!dungeon().inBounds(next)) continue;
                if (dungeon().blocksMove(next)) continue;
                if (dist[next.y][next.x] != -1) continue;
                if (next != player_.pos) {
                    bool blocked = false;
                    for (auto& m : monsters()) {
                        if (&m != &source && m.alive && m.pos == next) { blocked = true; break; }
                    }
                    if (blocked) continue;
                }
                dist[next.y][next.x] = dist[cur.y][cur.x] + 1;
                parent[next.y][next.x] = cur;
                q.push(next);
            }
        }
    }

    std::vector<Vec2> path;
    if (dist[player_.pos.y][player_.pos.x] < 0) return path;
    Vec2 cur = player_.pos;
    while (cur != source.pos) {
        path.push_back(cur);
        cur = parent[cur.y][cur.x];
        if (cur.x < 0) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void GameSession::monsterAttack(Monster& m) {
    int dmg = std::max(1, rng_.range(1, m.atk) - player_.defensePower());
    player_.stats.hp -= dmg;
    addMessage(std::string("The ") + m.name + std::string(" hits you for ") + std::to_string(dmg) + " damage!", Color::BrightRed);

    if (rng_.chance(12)) {
        if (m.damageType == DamageType::Poison) {
            player_.effects.add(EffectId::Poison, 6, 2, m.name);
            addMessage("You are poisoned!", Color::Green);
        } else if (m.damageType == DamageType::Fire) {
            player_.effects.add(EffectId::Burning, 4, 2, m.name);
            addMessage("You are burning!", Color::BrightRed);
        } else if (m.damageType == DamageType::Shadow) {
            if (rng_.chance(50)) {
                player_.effects.add(EffectId::Cursed, 10, 1, m.name);
                addMessage("You feel cursed!", Color::Purple);
            }
        } else if (m.damageType == DamageType::Frost) {
            player_.effects.add(EffectId::Chilled, 5, 1, m.name);
            addMessage("You are chilled to the bone!", Color::BrightCyan);
        }
    }

    if (player_.stats.hp <= 0) {
        player_.stats.hp = 0;
        dead_ = true;
        addMessage(std::string("You are slain by the ") + m.name + ".", Color::BrightRed);
    }
}

void GameSession::monsterTurns() {
    for (auto& m : monsters()) {
        if (!m.alive || dead_) continue;
        m.energy += m.speed;
        while (m.energy >= 100 && m.alive && !dead_) {
            m.energy -= 100;
            updateMonster(m);
        }
    }
}

void GameSession::updateMonster(Monster& m) {
    if (monsterCanSee(m)) {
        m.aware = true;
        m.lastSeenPlayer = player_.pos;
        m.aiState = AiState::Hunting;
    }

    if (m.pos.chebyshev(player_.pos) <= 1) {
        monsterAttack(m);
        return;
    }

    if (m.erratic && rng_.chance(45)) {
        wander(m);
        return;
    }

    if (!m.aware) {
        if (rng_.chance(15)) wander(m);
        return;
    }

    auto path = pathToPlayer(m);
    if (path.empty()) return;

    Vec2 next = path.front();
    if (dungeon().at(next) == Tile::DoorClosed && m.canOpenDoors) {
        dungeon().set(next, Tile::DoorOpen);
        return;
    }

    if (next == player_.pos) {
        monsterAttack(m);
        return;
    }

    m.pos = next;
}

void GameSession::wander(Monster& m) {
    std::array<Vec2, 8> dirs = {{{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}}};
    std::shuffle(dirs.begin(), dirs.end(), rng_.engine());
    for (auto& d : dirs) {
        Vec2 p = m.pos + d;
        if (!dungeon().walkable(p)) continue;
        if (p == player_.pos) continue;
        if (monsterAt(p)) continue;
        if (dungeon().at(p) == Tile::DoorClosed && m.canOpenDoors) {
            dungeon().set(p, Tile::DoorOpen);
            return;
        }
        if (dungeon().at(p) == Tile::DoorClosed) continue;
        m.pos = p;
        return;
    }
}

bool GameSession::waitTurn() {
    addMessage("You wait.", Color::Gray);
    return true;
}

bool GameSession::useItem(size_t idx) {
    if (idx >= player_.inventory.size()) return false;
    Item it = player_.inventory.at(idx);

    if (it.kind == ItemKind::PotionHeal) {
        int before = player_.stats.hp;
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + it.power);
        addMessage(std::string("You recover ") + std::to_string(player_.stats.hp - before) + " HP.", Color::BrightGreen);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionGreaterHeal) {
        int before = player_.stats.hp;
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + it.power);
        addMessage(std::string("You recover ") + std::to_string(player_.stats.hp - before) + " HP (greater).", Color::BrightGreen);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionMana) {
        player_.stats.saturation += 200;
        if (player_.stats.saturation > player_.stats.maxSaturation) player_.stats.saturation = player_.stats.maxSaturation;
        addMessage("Mana surges, saturation increases.", Color::BrightCyan);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionAntidote) {
        player_.effects.remove(EffectId::Poison);
        addMessage("The poison is purged.", Color::BrightGreen);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionFireResist) {
        player_.effects.add(EffectId::Shielded, 50, 5, "fire resist");
        addMessage("You feel resistant to fire.", Color::BrightRed);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionFrostResist) {
        player_.effects.add(EffectId::Shielded, 50, 5, "frost resist");
        addMessage("You feel resistant to frost.", Color::BrightCyan);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionShadowResist) {
        player_.effects.add(EffectId::Blessed, 40, 3, "shadow resist");
        addMessage("Shadow cannot touch you.", Color::Purple);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::PotionStrength) {
        player_.stats.strength++;
        player_.inventory.remove(idx);
        addMessage("You feel stronger.", Color::BrightMagenta);
        return true;
    }

    if (it.kind == ItemKind::PotionHaste) {
        player_.hasteTurns += std::max(10, it.power);
        player_.inventory.remove(idx);
        addMessage("The world seems to slow around you.", Color::BrightCyan);
        return true;
    }

    if (it.kind == ItemKind::PotionRejuvenation) {
        player_.stats.hp = player_.stats.maxHp;
        player_.stats.strength += 2;
        player_.stats.hunger = player_.stats.maxHunger;
        player_.stats.saturation = player_.stats.maxSaturation;
        player_.inventory.remove(idx);
        addMessage("Power surges through your body. Fully restored!", Color::BrightYellow);
        return true;
    }

    if (it.kind == ItemKind::PotionInvisibility) {
        player_.effects.add(EffectId::Invisible, it.power, 1, "potion");
        player_.inventory.remove(idx);
        addMessage("You fade from sight.", Color::Gray);
        return true;
    }

    if (it.kind == ItemKind::Food || it.kind == ItemKind::FoodBread || it.kind == ItemKind::FoodMeat || it.kind == ItemKind::FoodRation || it.kind == ItemKind::FoodFruit) {
        int before = player_.stats.hp;
        int nutrition = it.power * 10;
        int sat = it.power * 5;
        if (it.kind == ItemKind::FoodMeat) { nutrition += 100; sat += 50; }
        if (it.kind == ItemKind::FoodRation) { nutrition += 150; sat += 80; }
        if (it.kind == ItemKind::FoodFruit) { nutrition += 50; sat += 100; }
        player_.eatFood(nutrition, sat);
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + it.power / 2);
        player_.inventory.remove(idx);
        addMessage(std::string("You eat the ") + it.name + " and recover " + std::to_string(player_.stats.hp - before) + " HP. Hunger: " + player_.stats.hungerName(), Color::Brown);
        return true;
    }

    if (it.kind == ItemKind::Weapon || it.kind == ItemKind::WeaponRanged || it.kind == ItemKind::Armor || it.kind == ItemKind::Shield || it.kind == ItemKind::Helmet || it.kind == ItemKind::Boots || it.kind == ItemKind::Ring || it.kind == ItemKind::Amulet) {
        std::optional<Item> prev;
        bool ok = player_.equipment.equip(it, prev);
        if (!ok) {
            addMessage("You cannot equip that.", Color::Gray);
            return false;
        }
        player_.inventory.remove(idx);
        if (prev) player_.inventory.add(*prev);
        addMessage(std::string("You equip the ") + it.name + ".", Color::BrightCyan);
        codex_.addFragment("guide." + it.stableId, it.name);
        return true;
    }

    if (it.kind == ItemKind::GuideFragment) {
        handleGuideFragment(it);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::LoreScroll) {
        handleLoreScroll(it);
        player_.inventory.remove(idx);
        return true;
    }

    if (it.kind == ItemKind::ScrollLightning) {
        Monster* target = nullptr;
        int bestDist = std::numeric_limits<int>::max();
        for (auto& m : monsters()) {
            if (!m.alive) continue;
            if (!dungeon().visible[m.pos.y][m.pos.x]) continue;
            int d = m.pos.chebyshev(player_.pos);
            if (d < bestDist) { bestDist = d; target = &m; }
        }
        player_.inventory.remove(idx);
        if (!target) {
            addMessage("Lightning flashes harmlessly.", Color::Gray);
            return true;
        }
        int dmg = it.power + player_.stats.level * 2;
        target->hp -= dmg;
        target->aware = true;
        addMessage("Lightning strikes the " + target->name + std::string(" for ") + std::to_string(dmg) + " damage!", Color::BrightYellow);
        if (target->hp <= 0) killMonster(*target);
        return true;
    }

    if (it.kind == ItemKind::ScrollFireball) {
        player_.inventory.remove(idx);
        std::vector<Monster*> targets;
        for (auto& m : monsters()) if (m.alive && dungeon().visible[m.pos.y][m.pos.x]) targets.push_back(&m);
        if (targets.empty()) {
            addMessage("The flames find no target.", Color::Gray);
            return true;
        }
        for (auto* m : targets) {
            int dmg = it.power + player_.stats.level * 2;
            m->hp -= dmg;
            m->aware = true;
            addMessage("Fire engulfs the " + m->name + std::string(" for ") + std::to_string(dmg) + " damage!", Color::BrightRed);
            if (m->hp <= 0) killMonster(*m);
        }
        return true;
    }

    if (it.kind == ItemKind::ScrollTeleport) {
        player_.inventory.remove(idx);
        Vec2 p;
        for (int tries = 0; tries < 1000; ++tries) {
            p = randomWalkable();
            if (!monsterAt(p)) { player_.pos = p; break; }
        }
        computeFov();
        addMessage("Space twists around you.", Color::BrightMagenta);
        return true;
    }

    if (it.kind == ItemKind::ScrollMapping) {
        player_.inventory.remove(idx);
        for (int y = 0; y < dungeon().mapH; ++y) for (int x = 0; x < dungeon().mapW; ++x) dungeon().explored[y][x] = true;
        addMessage("The dungeon map unfolds in your mind.", Color::BrightCyan);
        return true;
    }

    if (it.kind == ItemKind::ScrollEnchant) {
        player_.inventory.remove(idx);
        if (player_.equipment.mainHand) {
            player_.equipment.mainHand->power += 1;
            addMessage("Your weapon glows brighter! Enchanted.", Color::Gold);
        } else {
            addMessage("The scroll fizzles - no weapon equipped.", Color::Gray);
        }
        return true;
    }

    if (it.kind == ItemKind::ScrollBanishment) {
        player_.inventory.remove(idx);
        int banished = 0;
        for (auto& m : monsters()) {
            if (m.alive && dungeon().visible[m.pos.y][m.pos.x] && !m.boss) {
                m.alive = false;
                banished++;
            }
        }
        addMessage(std::string("Banishment! ") + std::to_string(banished) + " creatures banished.", Color::Purple);
        return true;
    }

    if (it.kind == ItemKind::Bomb || it.kind == ItemKind::BombDynamite) {
        player_.inventory.remove(idx);
        int rad = (it.kind == ItemKind::BombDynamite) ? 3 : 2;
        int dmg = it.power;
        int hit = 0;
        for (auto& m : monsters()) {
            if (!m.alive) continue;
            if (m.pos.chebyshev(player_.pos) <= rad) {
                m.hp -= dmg;
                m.aware = true;
                hit++;
                if (m.hp <= 0) killMonster(m);
            }
        }
        addMessage(std::string("Boom! ") + std::to_string(hit) + " enemies caught in explosion for " + std::to_string(dmg) + " damage!", Color::BrightRed);
        return true;
    }

    if (it.kind == ItemKind::AmuletShadowdeep) {
        addMessage("The Amulet whispers of the surface.", Color::BrightYellow);
        return false;
    }

    return false;
}

bool GameSession::dropItem(size_t idx) {
    if (idx >= player_.inventory.size()) return false;
    Item it = player_.inventory.at(idx);
    if (it.kind == ItemKind::AmuletShadowdeep) player_.hasAmulet = false;
    it.pos = player_.pos;
    items().push_back(it);
    player_.inventory.remove(idx);
    addMessage(std::string("You drop the ") + it.name + ".", Color::Gray);
    return true;
}

bool GameSession::quaffPotion() {
    for (size_t i = 0; i < player_.inventory.size(); ++i) {
        auto k = player_.inventory.at(i).kind;
        if (k == ItemKind::PotionHeal || k == ItemKind::PotionGreaterHeal || k == ItemKind::PotionStrength || k == ItemKind::PotionHaste || k == ItemKind::PotionRejuvenation || k == ItemKind::PotionInvisibility || k == ItemKind::PotionMana || k == ItemKind::PotionAntidote || k == ItemKind::PotionFireResist || k == ItemKind::PotionFrostResist || k == ItemKind::PotionShadowResist) return useItem(i);
    }
    addMessage("You have no potions.", Color::Gray);
    return false;
}

bool GameSession::readScroll() {
    for (size_t i = 0; i < player_.inventory.size(); ++i) {
        auto k = player_.inventory.at(i).kind;
        if (k == ItemKind::ScrollLightning || k == ItemKind::ScrollFireball || k == ItemKind::ScrollTeleport || k == ItemKind::ScrollMapping || k == ItemKind::ScrollIdentify || k == ItemKind::ScrollEnchant || k == ItemKind::ScrollBanishment || k == ItemKind::ScrollSummon) return useItem(i);
    }
    addMessage("You have no scrolls.", Color::Gray);
    return false;
}

void GameSession::addMessage(const std::string& s, Color c) {
    if (!messages_.empty() && messages_.back().text == s) {
        messages_.back().count++;
        return;
    }
    messages_.push_back({s, c, 1});
    while (messages_.size() > 100) messages_.pop_front();
}

const std::deque<GameMessage>& GameSession::messages() const { return messages_; }

long long GameSession::kills() const { return kills_; }
long long GameSession::goldEarned() const { return goldEarned_; }
long long GameSession::startTimeMs() const { return startedMs_; }

std::string GameSession::elapsedString() const {
    long long sec = (nowMs() - startedMs_) / 1000;
    char b[64];
    std::snprintf(b, sizeof(b), "%lldm %02llds", sec / 60, sec % 60);
    return b;
}

void GameSession::setEntryId(const std::string& id) { entryId_ = id; }
std::string GameSession::entryId() const { return entryId_; }

}
