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
    for (int i = 0; i <= kMaxDepth; ++i) {
        floors_[i] = Floor{};
        floors_[i].depth = i;
        floors_[i].region = regionForDepth(i);
    }
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

    depth_ = 1;
    running_ = true;
    dead_ = false;
    won_ = false;
    kills_ = 0;
    goldEarned_ = 0;
    messages_.clear();
    startedMs_ = nowMs();

    resetFloors();

    Item dagger;
    dagger.stableId = "item.dagger";
    dagger.name = "dagger";
    dagger.kind = ItemKind::Weapon;
    dagger.glyph = '/';
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
        scroll.color = Color::BrightYellow;
        scroll.power = 18;
        player_.inventory.add(scroll);
    }

    ensureFloor(1);
    player_.pos = floors_[1].dungeon.stairsUp;

    addMessage("Welcome to the Shadowdeep.", Color::BrightYellow);
    addMessage("Recover the Amulet and return to the surface.", Color::BrightYellow);
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

Item GameSession::makeHealPotion(Vec2 pos) {
    Item i;
    i.stableId = "item.potion_heal";
    i.name = "healing potion";
    i.kind = ItemKind::PotionHeal;
    i.glyph = '!';
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

    if (r <= 18) {
        i = makeHealPotion(pos);
    } else if (r <= 22 && d >= 3) {
        i.stableId = "item.potion_strength";
        i.name = "potion of strength";
        i.kind = ItemKind::PotionStrength;
        i.glyph = '!';
        i.color = Color::BrightMagenta;
        i.power = 1;
    } else if (r <= 26 && d >= 3) {
        i.stableId = "item.potion_haste";
        i.name = "potion of haste";
        i.kind = ItemKind::PotionHaste;
        i.glyph = '!';
        i.color = Color::BrightCyan;
        i.power = 18;
    } else if (r <= 29 && d >= 6) {
        i.stableId = "item.potion_rejuv";
        i.name = "potion of rejuvenation";
        i.kind = ItemKind::PotionRejuvenation;
        i.glyph = '!';
        i.color = Color::BrightYellow;
    } else if (r <= 32 && d >= 8) {
        i.stableId = "item.potion_invis";
        i.name = "potion of invisibility";
        i.kind = ItemKind::PotionInvisibility;
        i.glyph = '!';
        i.color = Color::Gray;
        i.power = 20;
    } else if (r <= 48) {
        auto& tmpl = allItemTemplates()[rng_.range(0, 3)];
        i.stableId = tmpl.stableId;
        i.name = tmpl.name;
        i.kind = ItemKind::Weapon;
        i.glyph = tmpl.glyph;
        i.color = tmpl.color;
        i.power = tmpl.basePower + rng_.range(0, 2);
        i.slot = EquipSlot::MainHand;
        if (rng_.chance(15)) {
            Enchantment ench;
            ench.id = static_cast<EnchantmentId>(rng_.range(1, 12));
            ench.power = rng_.range(1, 3);
            i.enchantments.push_back(ench);
            i.rarity = ItemRarity::Uncommon;
        }
    } else if (r <= 60) {
        auto& tmpl = allItemTemplates()[rng_.range(5, 9)];
        i.stableId = tmpl.stableId;
        i.name = tmpl.name;
        i.kind = tmpl.kind;
        i.glyph = tmpl.glyph;
        i.color = tmpl.color;
        i.power = tmpl.basePower + rng_.range(0, 2);
        i.slot = tmpl.slot;
        if (rng_.chance(15)) {
            Enchantment ench;
            ench.id = EnchantmentId::Fortified;
            ench.power = rng_.range(1, 2);
            i.enchantments.push_back(ench);
            i.rarity = ItemRarity::Uncommon;
        }
    } else if (r <= 66) {
        i.stableId = "item.food";
        i.name = "ration of food";
        i.kind = ItemKind::Food;
        i.glyph = '%';
        i.color = Color::Brown;
        i.power = 8;
    } else if (r <= 72) {
        i.stableId = "item.gold";
        i.name = "pile of gold";
        i.kind = ItemKind::Gold;
        i.glyph = '$';
        i.color = Color::Gold;
        i.power = rng_.range(4, 8 + d * 3);
        i.valueGold = i.power;
    } else if (r <= 75) {
        i.stableId = "item.ruby";
        i.name = "ruby";
        i.kind = ItemKind::Ruby;
        i.glyph = '$';
        i.color = Color::BrightRed;
        i.power = 1;
        i.valueGold = rng_.range(1, 3);
    } else if (r <= 80) {
        i.stableId = "item.scroll_lightning";
        i.name = "scroll of lightning";
        i.kind = ItemKind::ScrollLightning;
        i.glyph = '?';
        i.color = Color::BrightYellow;
        i.power = 18 + d * 2;
    } else if (r <= 86 && d >= 3) {
        i.stableId = "item.scroll_fireball";
        i.name = "scroll of fireball";
        i.kind = ItemKind::ScrollFireball;
        i.glyph = '?';
        i.color = Color::BrightRed;
        i.power = 30 + d * 2;
    } else if (r <= 91) {
        i.stableId = "item.scroll_teleport";
        i.name = "scroll of teleport";
        i.kind = ItemKind::ScrollTeleport;
        i.glyph = '?';
        i.color = Color::BrightMagenta;
    } else if (r <= 96) {
        i.stableId = "item.scroll_mapping";
        i.name = "scroll of magic mapping";
        i.kind = ItemKind::ScrollMapping;
        i.glyph = '?';
        i.color = Color::BrightCyan;
    } else {
        i.stableId = "item.scroll_identify";
        i.name = "scroll of identify";
        i.kind = ItemKind::ScrollIdentify;
        i.glyph = '?';
        i.color = Color::White;
    }

    i.pos = pos;
    return i;
}

Vec2 GameSession::randomWalkable() {
    for (int n = 0; n < 3000; ++n) {
        Vec2 p{rng_.range(1, kMapW - 2), rng_.range(1, kMapH - 2)};
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

Monster GameSession::createMonster(const MonsterTemplate& t, Vec2 p) {
    Monster m;
    m.stableId = t.stableId;
    m.name = t.name;
    m.glyph = t.glyph;
    m.color = t.color;
    m.pos = p;
    m.hp = t.baseHp + rng_.range(0, t.hpExtra);
    m.maxHp = m.hp;
    m.atk = t.baseAtk;
    m.def = t.baseDef;
    m.xp = t.xp;
    m.speed = t.speed;
    m.erratic = t.erratic;
    m.boss = (t.stableId == "monster.ancient_dragon");
    m.canOpenDoors = t.canOpenDoors;
    m.canFly = t.canFly;
    m.family = t.family;
    m.damageType = t.damageType;
    m.id = rng_.range(1, 1000000);
    return m;
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

    int count = 5 + d / 2 + rng_.range(0, 3);
    if (d >= 20) count += 3;

    std::vector<const MonsterTemplate*> eligible;
    for (auto& t : allMonsterTemplates()) {
        if (t.stableId == "monster.ancient_dragon") continue;
        if (d >= t.minDepth && d <= t.maxDepth) eligible.push_back(&t);
    }

    for (int n = 0; n < count; ++n) {
        if (eligible.empty()) break;
        Vec2 p;
        for (int tries = 0; tries < 500; ++tries) {
            p = {rng_.range(1, kMapW - 2), rng_.range(1, kMapH - 2)};
            bool occupied = false;
            for (auto& m : f.monsters) if (m.alive && m.pos == p) occupied = true;
            if (f.dungeon.walkable(p) && !occupied && p.manhattan(f.dungeon.stairsUp) >= 6) break;
        }
        const MonsterTemplate* t = eligible[rng_.range(0, static_cast<int>(eligible.size()) - 1)];
        f.monsters.push_back(createMonster(*t, p));
    }

    int itemCount = 4 + rng_.range(0, 4) + d / 3;
    for (int n = 0; n < itemCount; ++n) {
        Vec2 p;
        for (int tries = 0; tries < 500; ++tries) {
            p = {rng_.range(1, kMapW - 2), rng_.range(1, kMapH - 2)};
            bool used = false;
            for (auto& it : f.items) if (it.pos == p) used = true;
            if (f.dungeon.walkable(p) && p != f.dungeon.stairsUp && p != f.dungeon.stairsDown && !used) break;
        }
        f.items.push_back(randomItem(p, d));
    }

    if (d == kMaxDepth) {
        Vec2 bossPos = f.dungeon.rooms.empty() ? f.dungeon.stairsDown : f.dungeon.rooms.back().center();
        if (bossPos == f.dungeon.stairsDown) bossPos = bossPos + Vec2{-1, 0};
        if (!f.dungeon.walkable(bossPos)) bossPos = f.dungeon.rooms.back().center();

        auto* dragonT = findMonsterTemplate("monster.ancient_dragon");
        if (dragonT) f.monsters.push_back(createMonster(*dragonT, bossPos));

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
        a.color = Color::Gold;
        a.pos = amuletPos;
        a.rarity = ItemRarity::Mythic;
        a.identified = true;
        f.items.push_back(a);
    }
}

void GameSession::computeFov() {
    dungeon().computeFov(player_.pos, kFovRadius);
}

bool GameSession::movePlayer(int dx, int dy) {
    Vec2 target = player_.pos + Vec2{dx, dy};
    if (!dungeon().inBounds(target)) return false;

    Monster* m = monsterAtPtr(target);
    if (m) {
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
    return true;
}

void GameSession::autoPickup() {
    for (int i = 0; i < static_cast<int>(items().size()); ++i) {
        if (items()[i].pos == player_.pos) {
            if (items()[i].kind == ItemKind::Gold) {
                int g = items()[i].valueGold;
                player_.stats.gold += g;
                goldEarned_ += g;
                addMessage("You pick up " + std::to_string(g) + " gold.", Color::Gold);
                items().erase(items().begin() + i);
                return;
            } else if (items()[i].kind == ItemKind::Ruby) {
                int r = items()[i].valueGold;
                player_.stats.rubies += r;
                addMessage("You pick up " + std::to_string(r) + " ruby.", Color::BrightRed);
                items().erase(items().begin() + i);
                return;
            }
        }
    }
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
        addMessage("You collect " + std::to_string(it.valueGold) + " gold.", Color::Gold);
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
        addMessage("Return to the surface.", Color::Gold);
    } else {
        addMessage("You pick up " + it.name + ".", it.color);
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
        addMessage("The dungeon goes no deeper.", Color::Gray);
        return false;
    }
    ++depth_;
    ensureFloor(depth_);
    player_.pos = dungeon().stairsUp;
    computeFov();
    addMessage("You descend to level " + std::to_string(depth_) + " (" + regionNameForDepth(depth_) + ").", Color::BrightYellow);
    if (depth_ == kMaxDepth) addMessage("A tremendous heat fills the darkness.", Color::BrightRed);
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
    addMessage("You ascend to level " + std::to_string(depth_) + ".", Color::BrightCyan);
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

    addMessage("You hit the " + m.name + " for " + std::to_string(damage) + " damage.", Color::White);

    if (m.hp <= 0) killMonster(m);
}

void GameSession::killMonster(Monster& m) {
    m.alive = false;
    ++kills_;
    addMessage("The " + m.name + " dies.", Color::BrightGreen);
    bool level = player_.gainXp(m.xp);
    addMessage("You gain " + std::to_string(m.xp) + " XP.", Color::Gray);
    if (level) addMessage("You advance to level " + std::to_string(player_.stats.level) + "!", Color::BrightYellow);

    if (!m.boss && rng_.chance(20)) {
        Item drop = randomItem(m.pos, depth_);
        items().push_back(drop);
    } else if (!m.boss && rng_.chance(25)) {
        Item gold;
        gold.stableId = "item.gold";
        gold.name = "pile of gold";
        gold.kind = ItemKind::Gold;
        gold.glyph = '$';
        gold.color = Color::Gold;
        gold.power = rng_.range(2, 6 + depth_ * 2);
        gold.valueGold = gold.power;
        gold.pos = m.pos;
        items().push_back(gold);
    }

    if (m.boss) addMessage("The guardian of the Amulet has fallen!", Color::BrightYellow);
}

void GameSession::endTurn() {
    ++player_.stats.turns;
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
        addMessage("You suffer " + std::to_string(dmg) + " poison damage.", Color::Green);
    }

    if (player_.effects.has(EffectId::Regeneration)) {
        int heal = player_.effects.getPower(EffectId::Regeneration);
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + heal);
    }

    if (!skipMonsterPhase) monsterTurns();

    if (player_.stats.hp <= 0) {
        dead_ = true;
        player_.stats.hp = 0;
    }
}

bool GameSession::monsterCanSee(const Monster& m) const {
    const Dungeon& d = floors_[depth_].dungeon;
    if (m.pos.chebyshev(player_.pos) > 14) return false;
    return d.lineVisible(m.pos, player_.pos);
}

std::vector<Vec2> GameSession::pathToPlayer(const Monster& source) {
    std::array<std::array<int, kMapW>, kMapH> dist;
    std::array<std::array<Vec2, kMapW>, kMapH> parent;
    for (auto& row : dist) row.fill(-1);

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
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void GameSession::monsterAttack(Monster& m) {
    int dmg = std::max(1, rng_.range(1, m.atk) - player_.defensePower());
    player_.stats.hp -= dmg;
    addMessage("The " + m.name + " hits you for " + std::to_string(dmg) + " damage!", Color::BrightRed);

    if (rng_.chance(10)) {
        if (m.damageType == DamageType::Poison) {
            player_.effects.add(EffectId::Poison, 5, 2, m.name);
            addMessage("You are poisoned!", Color::Green);
        } else if (m.damageType == DamageType::Fire) {
            player_.effects.add(EffectId::Burning, 4, 2, m.name);
            addMessage("You are burning!", Color::BrightRed);
        }
    }

    if (player_.stats.hp <= 0) {
        player_.stats.hp = 0;
        dead_ = true;
        addMessage("You are slain by the " + m.name + ".", Color::BrightRed);
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
        addMessage("You recover " + std::to_string(player_.stats.hp - before) + " HP.", Color::BrightGreen);
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
        player_.inventory.remove(idx);
        addMessage("Power surges through your body.", Color::BrightYellow);
        return true;
    }

    if (it.kind == ItemKind::Food) {
        int before = player_.stats.hp;
        player_.stats.hp = std::min(player_.stats.maxHp, player_.stats.hp + it.power);
        player_.inventory.remove(idx);
        addMessage("You eat the ration and recover " + std::to_string(player_.stats.hp - before) + " HP.", Color::BrightGreen);
        return true;
    }

    if (it.kind == ItemKind::Weapon || it.kind == ItemKind::Armor || it.kind == ItemKind::Shield || it.kind == ItemKind::Helmet || it.kind == ItemKind::Boots || it.kind == ItemKind::Ring || it.kind == ItemKind::Amulet) {
        std::optional<Item> prev;
        bool ok = player_.equipment.equip(it, prev);
        if (!ok) {
            addMessage("You cannot equip that.", Color::Gray);
            return false;
        }
        player_.inventory.remove(idx);
        if (prev) player_.inventory.add(*prev);
        addMessage("You equip the " + it.name + ".", Color::BrightCyan);
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
        addMessage("Lightning strikes the " + target->name + " for " + std::to_string(dmg) + " damage!", Color::BrightYellow);
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
            addMessage("Fire engulfs the " + m->name + " for " + std::to_string(dmg) + " damage!", Color::BrightRed);
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
        for (auto& row : dungeon().explored) row.fill(true);
        addMessage("The dungeon map unfolds in your mind.", Color::BrightCyan);
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
    addMessage("You drop the " + it.name + ".", Color::Gray);
    return true;
}

bool GameSession::quaffPotion() {
    for (size_t i = 0; i < player_.inventory.size(); ++i) {
        auto k = player_.inventory.at(i).kind;
        if (k == ItemKind::PotionHeal || k == ItemKind::PotionStrength || k == ItemKind::PotionHaste || k == ItemKind::PotionRejuvenation) return useItem(i);
    }
    addMessage("You have no potions.", Color::Gray);
    return false;
}

bool GameSession::readScroll() {
    for (size_t i = 0; i < player_.inventory.size(); ++i) {
        auto k = player_.inventory.at(i).kind;
        if (k == ItemKind::ScrollLightning || k == ItemKind::ScrollFireball || k == ItemKind::ScrollTeleport || k == ItemKind::ScrollMapping) return useItem(i);
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
