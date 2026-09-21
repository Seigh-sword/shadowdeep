#include "shadowdeep/app/app.hpp"
#include "shadowdeep/version.hpp"
#include "shadowdeep/tui/ascii_art.hpp"
#include "shadowdeep/world/tile.hpp"
#include "shadowdeep/entities/player.hpp"
#include "shadowdeep/update/update_manager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <cstdio>

namespace shadowdeep {

App::App(CliOptions opts) : opts_(std::move(opts)), screen_(120, 40) {}

bool App::init() {
    paths_ = getAppPaths();
    ensureAppDirs(paths_);
    terminal_ = createTerminalBackend();
    configMgr_ = std::make_unique<ConfigManager>(paths_);
    configMgr_->load();
    saveMgr_ = std::make_unique<SaveManager>(paths_);
    return true;
}

void App::shutdown() {
    if (terminal_) terminal_->restore();
}

void App::blit() {
    auto s = screen_.toAnsi(true);
    terminal_->writeRaw(s);
}

std::string App::promptString(const std::string& prompt, const std::string& initial) {
    std::string cur = initial;
    int cursor = static_cast<int>(cur.size());

    for (;;) {
        screen_.clear();
        screen_.drawBox(10, 5, 60, 7, Color::BrightCyan, true);
        screen_.text(12, 6, prompt, Color::BrightYellow, true);
        screen_.text(12, 8, cur + "_", Color::White, false);
        screen_.text(12, 10, "Enter confirm  Esc cancel", Color::Gray);
        blit();

        auto ev = terminal_->waitKey();
        if (ev.code == static_cast<int>(KeyCode::Escape)) return "";
        if (ev.code == static_cast<int>(KeyCode::Enter) || ev.code == '\r' || ev.code == '\n') {
            return cur;
        }
        if (ev.code == static_cast<int>(KeyCode::Backspace) || ev.code == 127) {
            if (!cur.empty() && cursor > 0) {
                cur.erase(cursor - 1, 1);
                cursor--;
            }
        } else if (ev.code >= 32 && ev.code <= 126) {
            if (cur.size() < 40) {
                cur.insert(cur.begin() + cursor, static_cast<char>(ev.code));
                cursor++;
            }
        } else if (ev.code == static_cast<int>(KeyCode::Left)) {
            if (cursor > 0) cursor--;
        } else if (ev.code == static_cast<int>(KeyCode::Right)) {
            if (cursor < static_cast<int>(cur.size())) cursor++;
        }
    }
}

bool App::confirm(const std::string& msg) {
    for (;;) {
        screen_.clear();
        screen_.drawBox(15, 8, 50, 6, Color::BrightRed, true);
        screen_.text(17, 9, msg, Color::BrightWhite, true);
        screen_.text(17, 11, "y/n", Color::Gray);
        blit();
        auto ev = terminal_->waitKey();
        if (ev.code == 'y' || ev.code == 'Y') return true;
        if (ev.code == 'n' || ev.code == 'N' || ev.code == static_cast<int>(KeyCode::Escape)) return false;
    }
}

void App::renderHome(int selected) {
    screen_.clear();
    int w = screen_.width();
    int h = screen_.height();

    auto& artLib = AsciiArtLibrary::instance();
    auto titleArt = artLib.getTitleArt();

    int titleW = titleArt.width;
    int titleX = (w - titleW) / 2;
    int titleY = 1;
    if (h > 30 && w > 90) {
        ascii_draw::drawSprite(screen_, titleX, titleY, titleArt);
        titleY += titleArt.height + 1;
    } else {
        std::string title = "S H A D O W D E E P";
        int tx = (w - static_cast<int>(title.size())) / 2;
        screen_.text(tx, 2, title, Color::BrightYellow, true);
        titleY = 4;
    }

    std::string ver = std::string(kGameVersion) + " - Infinite Depths - ASCII Art Edition";
    int vx = (w - static_cast<int>(ver.size())) / 2;
    screen_.text(vx, titleY, ver, Color::BrightCyan, true);

    int boxW = 34;
    int boxH = 14;
    int bx = (w - boxW) / 2;
    int by = titleY + 2;

    ascii_draw::drawBoxStyled(screen_, bx, by, boxW, boxH, Color::White, BoxStyle::Single, true);
    ascii_draw::drawBoxStyled(screen_, bx+1, by+1, boxW-2, boxH-2, Color::Gray, BoxStyle::Ascii, false);
    screen_.text(bx + 2, by, " MAIN MENU ", Color::BrightYellow, true);

    std::vector<std::string> items = {"Entries", "Codex / Guides", "Settings", "Changelog", "Credits", "Quit"};
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        int y = by + 2 + i;
        int x = bx + 3;
        bool sel = (i == selected);
        std::string prefix = sel ? "> " : "  ";
        Color c = sel ? Color::BrightWhite : Color::White;
        if (sel) {
            ascii_draw::drawBorderedText(screen_, x, y, prefix + items[i], Color::BrightWhite, Color::Gold, true);
        } else {
            screen_.text(x, y, prefix + items[i], c, sel);
        }
    }

    int decoY = by + boxH + 1;
    if (decoY + 6 < h) {
        auto goblinArt = artLib.getMonsterSprite("monster.goblin");
        ascii_draw::drawSprite(screen_, 4, decoY, goblinArt);
        auto dragonArt = artLib.getMonsterSprite("monster.dragon_young");
        ascii_draw::drawSprite(screen_, w - dragonArt.width - 4, decoY, dragonArt);
    }

    screen_.text(2, h - 3, "Up/Down W/S navigate  Enter select  Q quit  ASCII Art Library", Color::Gray);
    screen_.text(2, h - 2, "Colored ASCII letters only | Shop o | Altar a | Fountain f | Minimap m | Huge maps 160x80 viewport 80x24", Color::Gray);

    std::string author = "github.com/Seigh-sword";
    screen_.text(w - static_cast<int>(author.size()) - 2, h - 2, author, Color::Gray);
}

void App::renderEntries(const std::vector<EntryMetadata>& entries, int cursor, int scroll) {
    screen_.clear();
    int w = screen_.width();
    int h = screen_.height();

    screen_.text(2, 0, "Entries", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    int listH = h - 6;
    int visibleCount = listH - 1;

    if (entries.empty()) {
        screen_.text(2, 3, "(no entries) - press + to create", Color::Gray);
    } else {
        for (int i = 0; i < visibleCount; ++i) {
            int idx = scroll + i;
            if (idx >= static_cast<int>(entries.size())) break;
            auto& e = entries[idx];
            bool sel = (idx == cursor);
            std::string line;
            line += sel ? "> " : "  ";
            line += e.entryName.substr(0, 18);
            line += "  ";
            line += e.characterName.substr(0, 10);
            line += std::string("  Lv") + std::to_string(e.level);
            line += "  " + e.regionName.substr(0, 14);
            line += "  " + e.playTimeString();
            line += "  " + e.statusString();

            if (line.size() > static_cast<size_t>(w - 4)) line = line.substr(0, w - 4);

            Color c = sel ? Color::BrightWhite : (e.status == EntryStatus::Corrupted ? Color::BrightRed : Color::White);
            screen_.text(1, 2 + i, line, c, sel);
        }
    }

    int bottom = h - 4;
    screen_.drawHLine(0, bottom - 1, w, "-", Color::Gray);
    screen_.text(2, bottom, "+ New Entry", Color::BrightGreen, cursor == static_cast<int>(entries.size()));
    screen_.text(2, h - 2, "Enter Open  N New  R Rename  D Delete  B Backups  Esc Back", Color::Gray);
}

void App::renderNewEntryScreen(std::string& entryName, std::string& charName, int& pclass, uint32_t& seed, int field) {
    screen_.clear();
    int w = screen_.width();
    screen_.text(2, 0, "New Entry", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    std::vector<std::string> fields = {
        "Entry Name: " + entryName,
        "Character Name: " + charName,
        "Class: " + Player::className(static_cast<PlayerClass>(pclass)),
        std::string("Seed: ") + std::to_string(seed) + " (0=random)",
        "Create",
        "Back"
    };

    for (int i = 0; i < static_cast<int>(fields.size()); ++i) {
        bool sel = (i == field);
        screen_.text(4, 3 + i, (sel ? "> " : "  ") + fields[i], sel ? Color::BrightWhite : Color::White, sel);
    }

    screen_.text(2, screen_.height() - 2, "Up/Down navigate  Enter edit/select  Esc back", Color::Gray);
}

void App::renderGameplay(const GameSession& session) {
    screen_.clear();
    int w = screen_.width();
    int h = screen_.height();

    auto& p = session.player();
    auto& dungeon = session.dungeon();

    std::string title = std::string("SHADOWDEEP ") + std::string(kGameVersion) + " Depth " + std::to_string(session.depth()) + "/" + std::to_string(kOriginalMaxDepth) + (session.depth() > kOriginalMaxDepth ? "+" : "") + " " + regionNameForDepth(session.depth()) + " [" + dungeon.biomeName() + "] Seed " + std::to_string(session.rng().seedValue());
    if (session.isInfiniteDepth()) title += " INFINITE";

    screen_.text(0, 0, title.substr(0, w), Color::BrightCyan, true);

    std::string hpPart = "HP " + std::to_string(p.stats.hp) + "/" + std::to_string(p.stats.maxHp);
    std::string lvPart = "Lv " + std::to_string(p.stats.level);
    std::string xpPart = "XP " + std::to_string(p.stats.xp) + "/" + std::to_string(p.stats.xpNext);
    std::string atkPart = "Atk " + std::to_string(p.attackPower());
    std::string defPart = "Def " + std::to_string(p.defensePower());
    std::string goldPart = "Gold " + std::to_string(p.stats.gold);
    std::string rubyPart = "Rubies " + std::to_string(p.stats.rubies);
    std::string hungerPart = "Hunger " + p.stats.hungerName() + " (" + std::to_string(p.stats.hunger) + ") Sat " + std::to_string(p.stats.saturation);

    int yStatus = 1;
    int xCursor = 0;

    screen_.text(xCursor, yStatus, hpPart.substr(0, w - xCursor), p.stats.hp <= p.stats.maxHp / 3 ? Color::BrightRed : Color::BrightRed, true);
    xCursor += static_cast<int>(hpPart.size()) + 1;
    screen_.text(xCursor, yStatus, lvPart, Color::BrightYellow, true);
    xCursor += static_cast<int>(lvPart.size()) + 1;
    screen_.text(xCursor, yStatus, xpPart, Color::BrightMagenta, true);
    xCursor += static_cast<int>(xpPart.size()) + 1;
    screen_.text(xCursor, yStatus, atkPart, Color::BrightWhite, true);
    xCursor += static_cast<int>(atkPart.size()) + 1;
    screen_.text(xCursor, yStatus, defPart, Color::Steel, true);
    xCursor += static_cast<int>(defPart.size()) + 1;
    screen_.text(xCursor, yStatus, goldPart, Color::Gold, true);
    xCursor += static_cast<int>(goldPart.size()) + 1;
    screen_.text(xCursor, yStatus, rubyPart, Color::BrightRed, true);

    int yStatus2 = 2;
    std::string hungerColored = hungerPart;
    Color hungerColor = Color::Brown;
    if (p.stats.isStarving()) hungerColor = Color::BrightRed;
    else if (p.stats.isHungry()) hungerColor = Color::Yellow;
    else if (p.stats.hungerState() == HungerState::Sated || p.stats.hungerState() == HungerState::Full) hungerColor = Color::BrightGreen;
    screen_.text(0, yStatus2, hungerColored.substr(0, w), hungerColor, false);

    if (p.hasteTurns > 0) {
        std::string haste = " Haste " + std::to_string(p.hasteTurns);
        screen_.text(w - 20, yStatus2, haste, Color::BrightCyan, true);
    }
    if (!p.effects.all().empty()) {
        std::string eff = " [";
        for (auto& e : p.effects.all()) eff += e.displayName() + " ";
        eff += "]";
        screen_.text(0, yStatus2 + 1, eff.substr(0, w), Color::BrightMagenta);
    }

    int mapTop = 4;
    int mapLeft = 1;
    int mapBoxW = kViewportW + 2;
    int mapBoxH = kViewportH + 2;

    if (w < 90 || h < 30) {
        screen_.text(2, 6, "Terminal too small. Need at least 90x30.", Color::BrightRed, true);
        std::string sz = "Current: " + std::to_string(w) + "x" + std::to_string(h) + " Map: " + std::to_string(dungeon.mapW) + "x" + std::to_string(dungeon.mapH) + " Viewport: " + std::to_string(kViewportW) + "x" + std::to_string(kViewportH);
        screen_.text(2, 7, sz, Color::White);
        return;
    }

    screen_.drawBox(mapLeft, mapTop, mapBoxW, mapBoxH, Color::White, true);
    std::string mapTitle = " MAP Depth " + std::to_string(session.depth()) + " " + dungeon.biomeName() + " ";
    screen_.text(mapLeft + 2, mapTop, mapTitle.substr(0, mapBoxW - 4), dungeon.biomeColor(), true);

    int viewOriginX = dungeon.camera.pos.x;
    int viewOriginY = dungeon.camera.pos.y;

    for (int sy = 0; sy < kViewportH; ++sy) {
        for (int sx = 0; sx < kViewportW; ++sx) {
            Vec2 world{viewOriginX + sx, viewOriginY + sy};
            if (!dungeon.inBounds(world)) continue;
            if (!dungeon.explored[world.y][world.x]) continue;
            bool vis = dungeon.visible[world.y][world.x];
            Tile t = dungeon.at(world);
            char ch = tileGlyph(t);
            Color c = vis ? Color::Gray : Color::Blue;
            switch (t) {
                case Tile::Wall: c = vis ? Color::Steel : Color::Blue; break;
                case Tile::DoorClosed: c = vis ? Color::Brown : Color::Blue; break;
                case Tile::DoorOpen: c = vis ? Color::Brown : Color::Blue; break;
                case Tile::StairsDown: case Tile::StairsUp: c = vis ? Color::BrightYellow : Color::Blue; break;
                case Tile::Rubble: c = vis ? Color::Red : Color::Blue; break;
                case Tile::Water: c = vis ? Color::BrightCyan : Color::Blue; break;
                case Tile::Lava: c = vis ? Color::BrightRed : Color::Blue; break;
                case Tile::Fountain: c = vis ? Color::BrightCyan : Color::Blue; break;
                case Tile::Chest: c = vis ? Color::Gold : Color::Blue; break;
                case Tile::Altar: c = vis ? Color::BrightMagenta : Color::Blue; break;
                case Tile::Trap: c = vis ? Color::BrightRed : Color::Blue; break;
                default: break;
            }
            if (dungeon.biome == Biome::Fungal && vis) {
                if (t == Tile::Floor) c = Color::Green;
            } else if (dungeon.biome == Biome::Crystal && vis) {
                if (t == Tile::Floor) c = Color::BrightCyan;
            } else if (dungeon.biome == Biome::Infernal && vis) {
                if (t == Tile::Floor) c = Color::BrightRed;
            } else if (dungeon.biome == Biome::Void && vis) {
                if (t == Tile::Floor) c = Color::Purple;
            }
            screen_.put(mapLeft + 1 + sx, mapTop + 1 + sy, ch, c, vis);
        }
    }

    for (auto& it : session.items()) {
        if (!dungeon.inBounds(it.pos)) continue;
        if (!dungeon.visible[it.pos.y][it.pos.x]) continue;
        if (!dungeon.isInViewport(it.pos)) continue;
        Vec2 scr = dungeon.camera.worldToScreen(it.pos);
        screen_.set(mapLeft + 1 + scr.x, mapTop + 1 + scr.y, it.displayGlyph(), it.color, true);
    }

    for (auto& m : session.monsters()) {
        if (!m.alive) continue;
        if (!dungeon.inBounds(m.pos)) continue;
        if (!dungeon.visible[m.pos.y][m.pos.x]) continue;
        if (!dungeon.isInViewport(m.pos)) continue;
        Vec2 scr = dungeon.camera.worldToScreen(m.pos);
        screen_.set(mapLeft + 1 + scr.x, mapTop + 1 + scr.y, m.displayGlyph(), m.color, true);
    }

    if (dungeon.isInViewport(p.pos)) {
        Vec2 scr = dungeon.camera.worldToScreen(p.pos);
        screen_.put(mapLeft + 1 + scr.x, mapTop + 1 + scr.y, '@', Color::BrightWhite, true);
    }

    int sideLeft = mapLeft + mapBoxW + 1;
    int sideW = w - sideLeft - 1;
    if (sideW < 20) sideW = 20;
    int sideTop = mapTop;
    int sideH = mapBoxH;

    ascii_draw::drawBoxStyled(screen_, sideLeft, sideTop, sideW, sideH, Color::Steel, BoxStyle::Single, true);
    ascii_draw::drawBoxStyled(screen_, sideLeft+1, sideTop+1, sideW-2, sideH-2, Color::Gray, BoxStyle::Ascii, false);
    screen_.text(sideLeft + 2, sideTop, " INVENTORY & STATS - ASCII ", Color::BrightYellow, true);

    auto& inv = session.player().inventory.all();
    int invLines = std::min(6, static_cast<int>(inv.size()));
    screen_.text(sideLeft + 1, sideTop + 2, "Items:", Color::White, true);
    if (inv.empty()) {
        screen_.text(sideLeft + 1, sideTop + 3, "(empty)", Color::Gray);
    } else {
        for (int i = 0; i < invLines; ++i) {
            auto& it = inv[i];
            std::string s = std::string(1, static_cast<char>('a' + i)) + ") " + it.fullName();
            if (s.size() > static_cast<size_t>(sideW - 2)) s = s.substr(0, sideW - 2);
            screen_.text(sideLeft + 1, sideTop + 3 + i, s, it.color, false);
        }
        if (static_cast<int>(inv.size()) > invLines) {
            screen_.text(sideLeft + 1, sideTop + 3 + invLines, "... +" + std::to_string(inv.size() - invLines) + " more (i)", Color::Gray);
        }
    }

    int eqY = sideTop + 9;
    auto& eq = session.player().equipment;
    screen_.text(sideLeft + 1, eqY, "Equipment:", Color::White, true);
    std::string weapon = "W: " + std::string(eq.mainHand ? eq.mainHand->fullName() : "fists");
    screen_.text(sideLeft + 1, eqY + 1, weapon.substr(0, sideW - 2), Color::Steel);
    std::string armor = "A: " + std::string(eq.body ? eq.body->fullName() : "none");
    screen_.text(sideLeft + 1, eqY + 2, armor.substr(0, sideW - 2), Color::Steel);
    std::string shield = "D: " + std::string(eq.offHand ? eq.offHand->fullName() : "none");
    screen_.text(sideLeft + 1, eqY + 3, shield.substr(0, sideW - 2), Color::Steel);
    std::string helm = "H: " + std::string(eq.head ? eq.head->fullName() : "none");
    screen_.text(sideLeft + 1, eqY + 4, helm.substr(0, sideW - 2), Color::Steel);

    int codexY = eqY + 6;
    screen_.text(sideLeft + 1, codexY, "Codex:", Color::BrightCyan, true);
    std::string codexLine = std::to_string(session.codex().unlockedCount()) + "/" + std::to_string(session.codex().totalGuides()) + " guides";
    screen_.text(sideLeft + 1, codexY + 1, codexLine.substr(0, sideW - 2), Color::White);
    std::string loreLine = std::to_string(session.codex().loreCount()) + " lore found";
    screen_.text(sideLeft + 1, codexY + 2, loreLine.substr(0, sideW - 2), Color::Gold);

    int roomY = codexY + 4;
    screen_.text(sideLeft + 1, roomY, "Map Info:", Color::White, true);
    std::string mapInfo = std::to_string(dungeon.mapW) + "x" + std::to_string(dungeon.mapH) + " view " + std::to_string(viewOriginX) + "," + std::to_string(viewOriginY);
    screen_.text(sideLeft + 1, roomY + 1, mapInfo.substr(0, sideW - 2), Color::Gray);
    std::string roomsInfo = "Rooms: " + std::to_string(dungeon.rooms.size());
    screen_.text(sideLeft + 1, roomY + 2, roomsInfo.substr(0, sideW - 2), Color::Gray);
    if (session.isInfiniteDepth()) {
        screen_.text(sideLeft + 1, roomY + 3, "INFINITE DEPTH!", Color::Purple, true);
    }

    int miniY = roomY + 4;
    if (sideW >= 22 && miniY + 12 < sideTop + sideH - 6) {
        screen_.text(sideLeft + 1, miniY, "Minimap:", Color::BrightCyan, true);
        std::string mini = session.minimapString(18, 8);
        std::string curLine;
        int curRow = 0;
        for (char ch : mini) {
            if (ch == '\n') {
                if (curRow < 8) {
                    screen_.text(sideLeft + 1, miniY + 1 + curRow, curLine.substr(0, sideW - 2), Color::Gray);
                    curRow++;
                }
                curLine.clear();
            } else {
                curLine += ch;
            }
        }
        // ASCII art for nearby monster if visible
        if (sideW > 30) {
            for (auto& m : session.monsters()) {
                if (!m.alive) continue;
                if (!dungeon.visible[m.pos.y][m.pos.x]) continue;
                if (m.pos.chebyshev(p.pos) <= 5) {
                    auto& artLib = AsciiArtLibrary::instance();
                    auto sprite = artLib.getMonsterSprite(m.stableId);
                    if (sprite.width <= sideW - 2 && miniY + 10 + sprite.height < sideTop + sideH) {
                        ascii_draw::drawBoxStyled(screen_, sideLeft + 1, miniY + 9, sprite.width + 2, sprite.height + 2, m.color, BoxStyle::Single, false);
                        ascii_draw::drawSprite(screen_, sideLeft + 2, miniY + 10, sprite);
                        screen_.text(sideLeft + 1, miniY + 9 + sprite.height + 2, m.name.substr(0, sideW - 2), m.color, true);
                        ascii_draw::drawProgressBar(screen_, sideLeft + 1, miniY + 10 + sprite.height + 2, sideW - 2, m.hp, m.maxHp, ascii_draw::healthColor(m.hp, m.maxHp), Color::Gray, Color::White);
                        break;
                    }
                }
            }
        }
    }

    if (session.isInShop()) {
        screen_.text(sideLeft + 1, sideTop + sideH - 4, "SHOP HERE! Press o", Color::Gold, true);
        std::string goldInfo = "Gold: " + std::to_string(session.player().stats.gold);
        screen_.text(sideLeft + 1, sideTop + sideH - 3, goldInfo.substr(0, sideW - 2), Color::Gold);
    }

    Tile curTile = dungeon.at(session.player().pos);
    if (curTile == Tile::Altar) {
        screen_.text(sideLeft + 1, sideTop + sideH - 2, "Altar! Press a", Color::BrightMagenta, true);
    } else if (curTile == Tile::Fountain) {
        screen_.text(sideLeft + 1, sideTop + sideH - 2, "Fountain! Press f", Color::BrightCyan, true);
    } else if (curTile == Tile::Chest) {
        screen_.text(sideLeft + 1, sideTop + sideH - 2, "Chest! Move onto", Color::Gold, true);
    }

    // Health bar in side panel using ascii art library
    if (sideTop + sideH - 6 > codexY) {
        ascii_draw::drawProgressBar(screen_, sideLeft + 1, sideTop + sideH - 6, sideW - 2, p.stats.hp, p.stats.maxHp, ascii_draw::healthColor(p.stats.hp, p.stats.maxHp), Color::Gray, Color::White);
    }

    int msgTop = mapTop + mapBoxH + 1;
    int msgBoxH = h - msgTop - 2;
    if (msgBoxH < 4) msgBoxH = 4;
    screen_.drawBox(mapLeft, msgTop, w - mapLeft - 1, msgBoxH, Color::Gray, true);
    screen_.text(mapLeft + 2, msgTop, " MESSAGES ", Color::BrightYellow, true);

    int msgCount = std::min(msgBoxH - 2, static_cast<int>(session.messages().size()));
    int start = static_cast<int>(session.messages().size()) - msgCount;
    for (int i = 0; i < msgCount; ++i) {
        auto& msg = session.messages()[start + i];
        std::string txt = msg.text;
        if (msg.count > 1) txt += std::string(" (x") + std::to_string(msg.count) + ")";
        if (txt.size() > static_cast<size_t>(w - mapLeft - 4)) txt = txt.substr(0, w - mapLeft - 4);
        screen_.text(mapLeft + 2, msgTop + 1 + i, txt, msg.color);
    }

    screen_.text(2, h - 1, "hjkl move g:get i:inv c:codex b:shop a:altar f:fountain m:minimap q:quaff r:read </> stairs ?:help", Color::Gray);
}

void App::renderInventory(GameSession& session, int cursor) {
    int w = 80;
    int h = std::min(screen_.height() - 4, std::max(22, static_cast<int>(session.player().inventory.size()) + 14));
    int top = (screen_.height() - h) / 2;
    int left = (screen_.width() - w) / 2;

    ascii_draw::drawShadowBox(screen_, left, top, w, h, Color::BrightCyan, Color::Gray);
    screen_.text(left + 2, top, " INVENTORY - ASCII Art View ", Color::BrightYellow, true);

    auto& inv = session.player().inventory.all();
    auto& artLib = AsciiArtLibrary::instance();

    if (inv.empty()) {
        screen_.text(left + 3, top + 2, "(empty) - explore to find items, guide fragments, lore", Color::Gray);
    } else {
        int maxRows = h - 10;
        for (int i = 0; i < static_cast<int>(inv.size()) && i < maxRows; ++i) {
            auto& it = inv[i];
            std::string s = (i == cursor ? "> " : "  ");
            s += static_cast<char>('a' + i);
            s += ") ";
            s += it.fullName();
            s += " [" + std::to_string(static_cast<int>(it.rarity)) + "]";
            if (it.kind == ItemKind::GuideFragment) s += " - Enter unlock guide";
            if (it.kind == ItemKind::LoreScroll) s += " - lore";
            if (s.size() > static_cast<size_t>(w - 28)) s = s.substr(0, w - 28);
            screen_.text(left + 2, top + 2 + i, s, it.color, i == cursor);
        }
        if (cursor >= 0 && cursor < static_cast<int>(inv.size())) {
            auto& sel = inv[cursor];
            auto sprite = artLib.getItemSprite(sel.stableId);
            int artX = left + w - sprite.width - 3;
            int artY = top + 2;
            ascii_draw::drawBoxStyled(screen_, artX - 1, artY - 1, sprite.width + 2, sprite.height + 2, sel.color, BoxStyle::Single, false);
            ascii_draw::drawSprite(screen_, artX, artY, sprite);
            screen_.text(artX, artY + sprite.height + 1, sel.name.substr(0, sprite.width + 2), sel.color, true);
            std::string rarityStr = "Rarity: " + std::to_string(static_cast<int>(sel.rarity));
            screen_.text(artX, artY + sprite.height + 2, rarityStr.substr(0, sprite.width + 2), ascii_draw::rarityColor(static_cast<int>(sel.rarity)), false);
        }
    }

    auto& eq = session.player().equipment;
    std::string weapon = "Weapon: ";
    weapon += eq.mainHand ? eq.mainHand->fullName() : "bare fists";
    screen_.text(left + 2, top + h - 6, weapon.substr(0, w - 30), Color::Steel);

    std::string armor = "Armour: ";
    armor += eq.body ? eq.body->fullName() : "no armour";
    screen_.text(left + 2, top + h - 5, armor.substr(0, w - 30), Color::Steel);

    screen_.text(left + 2, top + h - 4, "Hunger: " + session.player().stats.hungerName() + " Sat: " + std::to_string(session.player().stats.saturation), Color::Brown);
    ascii_draw::drawProgressBar(screen_, left + 2, top + h - 3, 30, session.player().stats.hp, session.player().stats.maxHp, Color::BrightRed, Color::Gray, Color::White);
    screen_.text(left + 35, top + h - 3, "HP", Color::BrightRed, true);
    screen_.text(left + 2, top + h - 2, "Enter/use x/drop a-z quick use i/Esc close  ASCII Art Library", Color::Gray);
}

void App::renderHelp() {
    screen_.clear();
    int w = 72;
    int h = 28;
    int top = (screen_.height() - h) / 2;
    int left = (screen_.width() - w) / 2;

    screen_.drawBox(left, top, w, h, Color::BrightYellow, true);
    screen_.text(left + 2, top, " HELP - Essentials ", Color::BrightYellow, true);
    std::vector<std::string> lines = {
        "SHADOWDEEP " + std::string(kGameVersion) + " - Infinite Depths Edition",
        "",
        "GOAL: Depth 30 has Amulet. Return to surface. Beyond is infinite.",
        "",
        "MOVEMENT:",
        "  Arrow keys / hjkl / yubn   Move, attack, diagonal",
        "  > / <                      Descend / Ascend stairs",
        "  z or .                     Wait a turn",
        "",
        "ACTIONS:",
        "  g or ,                     Pick up item",
        "  i                          Inventory (side panel)",
        "  c                          Codex / Guides / Lore",
        "  q                          Quaff potion (auto)",
        "  r                          Read scroll (auto)",
        "  b                          Browse shop (when in shop)",
        "  a                          Pray at altar (nearby)",
        "  f                          Drink fountain (nearby)",
        "  m                          Toggle minimap overlay",
        "",
        "SURVIVAL:",
        "  Hunger drains. Saturation heals HP slowly.",
        "  Eat food (%). Starving damages you.",
        "  Collect ; guide fragments to unlock guides.",
        "  Bosses drop epic fragments + lore scrolls (?).",
        "  Altars A: sacrifice gold for blessing",
        "  Fountains F: drink for random effect",
        "  Chests C may be mimics! Careful!",
        "  Shops: find shop room, buy with gold (b)",
        "",
        "MAP:",
        "  Huge maps 160x80 (grows to 200x200 deep). Viewport shows portion.",
        "  Camera follows you. Biomes change visuals.",
        "  Minimap shows explored layout scaled down.",
        "  Rooms: shop, altar, fountain, chest, trap",
        "",
        "COLORS: HP Red  Lv Yellow  XP Magenta  Atk White  Def Gray  Gold Gold  Rubies Red",
        "  Hunger Brown->Green when sated. Biome colors vary.",
        "  Shop Gold  Altar Magenta  Fountain Cyan  Mimic Brown",
        "",
        "Press any key to return."
    };

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        Color c = Color::White;
        bool bold = false;
        if (i == 0) { c = Color::BrightYellow; bold = true; }
        else if (lines[i].rfind("GOAL:",0)==0 || lines[i].rfind("MOVEMENT:",0)==0 || lines[i].rfind("ACTIONS:",0)==0 || lines[i].rfind("SURVIVAL:",0)==0 || lines[i].rfind("MAP:",0)==0 || lines[i].rfind("COLORS:",0)==0) { c = Color::BrightCyan; bold = true; }
        screen_.text(left + 2, top + 1 + i, lines[i].substr(0, w - 4), c, bold);
    }
}

void App::renderCodex(GameSession& session, int tab, int cursor) {
    screen_.clear();
    int w = screen_.width();
    int h = screen_.height();

    ascii_draw::drawBoxStyled(screen_, 0, 0, w, h, Color::BrightYellow, BoxStyle::Single, true);
    screen_.text(2, 0, " Codex - Guides & Lore - ASCII Art Library ", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    std::vector<std::string> tabs = {"Guides", "Lore", "Fragments", "Biomes", "Bestiary"};
    int tx = 2;
    for (int i = 0; i < static_cast<int>(tabs.size()); ++i) {
        bool sel = (i == tab);
        std::string t = sel ? "[" + tabs[i] + "]" : " " + tabs[i] + " ";
        screen_.text(tx, 2, t, sel ? Color::BrightWhite : Color::Gray, sel);
        tx += static_cast<int>(t.size()) + 2;
    }

    int listTop = 4;
    int listH = h - 8;
    auto& artLib = AsciiArtLibrary::instance();

    if (tab == 0) {
        auto guides = session.codex().allGuides();
        std::sort(guides.begin(), guides.end(), [](auto& a, auto& b){ return a.title < b.title; });
        int visible = std::min(listH, static_cast<int>(guides.size()));
        for (int i = 0; i < visible; ++i) {
            int idx = i;
            if (idx >= static_cast<int>(guides.size())) break;
            auto& g = guides[idx];
            bool sel = (idx == cursor);
            std::string line = (sel ? "> " : "  ") + g.title + (g.unlocked ? " [UNLOCKED]" : " [LOCKED]") + " - " + session.codex().categoryName(g.category);
            if (line.size() > static_cast<size_t>(w - 30)) line = line.substr(0, w - 30);
            Color c = g.unlocked ? Color::BrightGreen : Color::Gray;
            if (sel) c = Color::BrightWhite;
            screen_.text(2, listTop + i, line, c, sel);
            if (sel && g.unlocked) {
                ascii_draw::drawWrappedText(screen_, 2, listTop + visible + 1, w - 4, 3, g.description, Color::White, false);
                if (w > 80) {
                    auto itemSprite = artLib.getItemSprite("item.guide_fragment_common");
                    ascii_draw::drawSprite(screen_, w - itemSprite.width - 4, listTop + 1, itemSprite);
                }
            }
        }
    } else if (tab == 1) {
        auto lores = session.codex().allLore();
        std::sort(lores.begin(), lores.end(), [](auto& a, auto& b){ return a.depthFound < b.depthFound; });
        int visible = std::min(listH, static_cast<int>(lores.size()));
        for (int i = 0; i < visible; ++i) {
            int idx = i;
            if (idx >= static_cast<int>(lores.size())) break;
            auto& l = lores[idx];
            bool sel = (idx == cursor);
            std::string line = (sel ? "> " : "  ") + l.title + " (Depth " + std::to_string(l.depthFound) + ")" + (l.isBossDrop ? " [BOSS]" : "");
            if (line.size() > static_cast<size_t>(w - 30)) line = line.substr(0, w - 30);
            Color c = l.isBossDrop ? Color::Gold : Color::White;
            if (sel) c = Color::BrightWhite;
            screen_.text(2, listTop + i, line, c, sel);
            if (sel) {
                ascii_draw::drawWrappedText(screen_, 2, listTop + visible + 1, w - 4, 3, l.text, Color::BrightYellow, false);
            }
        }
    } else if (tab == 2) {
        screen_.text(2, listTop, "Total fragments collected: " + std::to_string(session.codex().totalFragments()), Color::BrightCyan, true);
        screen_.text(2, listTop + 2, "Find G fragments in dungeon. Bosses drop epic fragments.", Color::White);
        screen_.text(2, listTop + 3, "Use fragment from inventory to unlock guides.", Color::Gray);
        screen_.text(2, listTop + 5, "Guides unlocked: " + std::to_string(session.codex().unlockedCount()) + "/" + std::to_string(session.codex().totalGuides()), Color::BrightGreen);
        if (w > 70) {
            auto fragArt = artLib.getItemSprite("item.guide_fragment_rare");
            ascii_draw::drawSprite(screen_, w - fragArt.width - 10, listTop, fragArt);
            auto loreArt = artLib.getItemSprite("item.lore_scroll");
            ascii_draw::drawSprite(screen_, w - loreArt.width - 10, listTop + fragArt.height + 2, loreArt);
        }
    } else if (tab == 3) {
        std::vector<std::string> biomes = {"Stone Depths - Gray stone, basic", "Fungal Bloom - Green, poison", "Crystal Caverns - Cyan, magic", "Infernal Foundry - Red, fire", "Abyssal Temple - Purple, shadow", "Flooded Halls - Cyan, water", "Frozen Vault - White, frost", "Overgrown Ruins - Green, nature", "Ancient Ruins - Brown, old", "Void Tear - Magenta, infinite depth"};
        for (int i = 0; i < static_cast<int>(biomes.size()) && i < listH; ++i) {
            screen_.text(2, listTop + i, biomes[i], Color::White);
        }
        if (w > 80) {
            for (int i = 0; i < 5 && i < static_cast<int>(biomes.size()); ++i) {
                std::string bName = "stone";
                if (i == 1) bName = "fungal";
                auto banner = artLib.getBiomeBanner(bName);
                if (banner.width < 30) {
                    ascii_draw::drawSprite(screen_, w - 35, listTop + i * 2, banner);
                }
            }
        }
    } else {
        std::vector<std::string> monsterIds = {"monster.giant_rat", "monster.goblin", "monster.skeleton", "monster.orc", "monster.troll", "monster.dragon_young", "monster.ancient_dragon", "monster.goblin_king", "monster.lich_king", "monster.demon_lord", "monster.shopkeeper", "monster.mimic"};
        int visible = std::min(listH, static_cast<int>(monsterIds.size()));
        int cIdx = std::clamp(cursor, 0, static_cast<int>(monsterIds.size()) - 1);
        for (int i = 0; i < visible; ++i) {
            int idx = i;
            if (idx >= static_cast<int>(monsterIds.size())) break;
            std::string mid = monsterIds[idx];
            bool sel = (idx == cIdx);
            std::string line = (sel ? "> " : "  ") + mid;
            screen_.text(2, listTop + i, line, sel ? Color::BrightWhite : Color::White, sel);
            if (sel && w > 60) {
                auto sprite = artLib.getMonsterSprite(mid);
                int sx = w - sprite.width - 4;
                int sy = listTop;
                ascii_draw::drawBoxStyled(screen_, sx - 1, sy - 1, sprite.width + 2, sprite.height + 2, sprite.fg, BoxStyle::Single, false);
                ascii_draw::drawSprite(screen_, sx, sy, sprite);
                screen_.text(sx, sy + sprite.height + 1, mid.substr(0, sprite.width + 2), sprite.fg, true);
            }
        }
    }

    ascii_draw::drawBoxStyled(screen_, 1, h - 3, w - 2, 3, Color::Gray, BoxStyle::Ascii, false);
    screen_.text(2, h - 2, "Tab switch category  Up/Down navigate  Esc back  ASCII Art Library draws all chars", Color::Gray);
}

void App::renderSettings() {
    screen_.clear();
    int w = screen_.width();
    screen_.text(2, 0, "Settings", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    auto& cfg = configMgr_->get();
    std::vector<std::string> lines = {
        std::string("Theme: ") + cfg.theme,
        std::string("Animations: ") + std::string(cfg.animations ? "On" : "Off"),
        std::string("Reduced Motion: ") + std::string(cfg.reducedMotion ? "On" : "Off"),
        std::string("Screen Shake: ") + std::string(cfg.screenShake ? "On" : "Off"),
        std::string("High Contrast: ") + std::string(cfg.highContrast ? "On" : "Off"),
        std::string("Mouse Support: ") + std::string(cfg.mouseSupport ? "On" : "Off"),
        std::string("Auto Pickup Gold: ") + std::string(cfg.autoPickupGold ? "On" : "Off"),
        std::string("Auto Check Update: ") + std::string(cfg.autoCheckUpdate ? "On" : "Off"),
        std::string("Update Channel: ") + cfg.updateChannel,
        "",
        "Press Esc to return"
    };

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        screen_.text(4, 3 + i, lines[i], Color::White);
    }
}

void App::renderChangelog() {
    screen_.clear();
    int w = screen_.width();
    screen_.text(2, 0, std::string("Changelog - ") + std::string(kGameVersion) + " Infinite Depths", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    std::vector<std::string> lines = {
        "Zv1 Infinite Depths Update - Crazy Edition",
        "",
        "Added:",
        "  - 150+ items: weapons, armor, potions, food, scrolls, guide fragments, lore",
        "  - 80+ monsters: 50 new enemies + 8 new bosses + shopkeeper + mimics",
        "  - Infinite depth beyond 30 with scaling (+power per depth, maps grow to 200x200)",
        "  - Huge maps 160x80 with viewport camera (only portion visible)",
        "  - Biomes: Stone, Fungal, Crystal, Infernal, Abyssal, Flooded, Frozen, Ruins, Void",
        "  - TUI borders with box-drawing, colors for HP/Lv/XP/Atk/Def/Gold/Rubies",
        "  - Hunger/saturation system: hunger drains, saturation heals HP, starving damages",
        "  - Guide fragments collectible unlocking codex guides",
        "  - Lore scrolls and boss lore drops with cutscenes",
        "  - Side inventory panel, top/bottom room info, improved layout",
        "  - Unicode glyphs: monsters and items use emoji/extended (displayGlyph)",
        "  - Shop rooms: random room becomes shop with shopkeeper and guard, buy with gold (o)",
        "  - Mimics: chest tiles 25% are mimics, disguised until approached, chest interaction",
        "  - Altar effects: sacrifice gold for blessing, stat boost, fragments, or curse (a)",
        "  - Fountain effects: heal, saturation, poison, max HP, hidden treasure (f)",
        "  - Minimap: scaled explored view in side panel + overlay (m)",
        "  - Random events: swarms, caches, whispers, treasure sense every ~30 turns",
        "  - Predictable release URLs: https://github.com/.../download/(version)/(file)",
        "  - Direct URL fallback in update checker + auto-update --auto-update",
        "  - Auto-install: Linux tar xzf and Windows batch updater with .bak backup",
        "",
        "Changed:",
        "  - Map size from 80x24 to 160x80, viewport 80x24",
        "  - Max depth from 30 to 200, original win at 30 still",
        "  - Help menu essentials only + shop/altar/fountain/minimap keys",
        "  - Food system expanded with bread, meat, rations, fruit",
        "  - Rendering uses screen_.set with displayGlyph for Unicode",
        "",
        "Fixed:",
        "  - MSYS2 cstdint include, generator unused variable, paths xdgOrHome guard",
        "  - macOS runner macos-13 -> macos-14, release workflow log capture",
        "  - monster.cpp double comma fix for unicodeGlyph insertion",
        "  - cli.cpp missing kMapW/kMapH includes",
        "",
        "Press any key to return"
    };

    for (int i = 0; i < static_cast<int>(lines.size()) && i < screen_.height() - 4; ++i) {
        Color c = Color::White;
        if (lines[i].rfind("Added:", 0) == 0 || lines[i].rfind("Changed:", 0) == 0 || lines[i].rfind("Fixed:", 0) == 0) c = Color::BrightCyan;
        if (i == 0) c = Color::BrightYellow;
        screen_.text(2, 2 + i, lines[i].substr(0, w - 4), c, i == 0);
    }
}

void App::renderCredits() {
    screen_.clear();
    int w = screen_.width();
    int h = screen_.height();
    screen_.text(2, 0, "Credits", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    std::vector<std::string> lines = {
        "SHADOWDEEP - Infinite Depths",
        "A roguelike dungeon crawler in C++20",
        "",
        "Made by:",
        "github.com/Seigh-sword",
        "",
        "Repository:",
        "https://github.com/Seigh-sword/shadowdeep",
        "Releases: https://github.com/Seigh-sword/shadowdeep/releases",
        "Direct download: https://github.com/Seigh-sword/shadowdeep/releases/download/(version)/(file)",
        "",
        "Licensed under the ISC License.",
        "",
        std::string("Version: ") + std::string(kGameVersion) + std::string(" (") + std::string(kVersionHuman) + ")",
        std::string("Save schema: ") + std::to_string(kSaveSchemaVersion),
        std::string("Config schema: ") + std::to_string(kConfigSchemaVersion),
        std::string("Build revision: ") + std::to_string(kBuildRevision),
        std::string("Map: ") + std::to_string(kMapW) + "x" + std::to_string(kMapH) + " Viewport: " + std::to_string(kViewportW) + "x" + std::to_string(kViewportH),
        std::string("Max Depth: ") + std::to_string(kMaxDepth) + " (Win at " + std::to_string(kOriginalMaxDepth) + ")",
        "",
        "Third-party: fmt MIT, spdlog MIT, nlohmann/json MIT, FTXUI MIT, cpr/libcurl MIT, Catch2 BSL-1.0",
        "",
        "Press any key to return"
    };

    for (int i = 0; i < static_cast<int>(lines.size()) && i < h - 3; ++i) {
        Color c = Color::White;
        if (i == 0) c = Color::BrightYellow;
        if (lines[i].find("github.com") != std::string::npos || lines[i].find("https://") != std::string::npos) c = Color::BrightCyan;
        screen_.text(4, 2 + i, lines[i].substr(0, w - 8), c, i == 0);
    }
}

void App::renderMessageLog(const GameSession& session) {
    screen_.clear();
    screen_.text(2, 0, "Message Log", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, screen_.width(), "-", Color::Gray);

    int y = 2;
    for (auto it = session.messages().rbegin(); it != session.messages().rend() && y < screen_.height() - 2; ++it) {
        std::string txt = it->text;
        if (it->count > 1) txt += std::string(" (x") + std::to_string(it->count) + ")";
        screen_.text(2, y++, txt.substr(0, screen_.width() - 4), it->color);
    }

    screen_.text(2, screen_.height() - 2, "Press any key to return", Color::Gray);
}

void App::showDeathScreen(const GameSession& session) {
    terminal_->restore();
    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;31mSHADOWDEEP\n\nYOU HAVE DIED\033[0m\n\n";
    std::cout << "Character: " << session.player().name << " (" << session.player().className() << ")\n";
    std::cout << "Depth reached   : " << session.depth() << (session.isInfiniteDepth() ? " (INFINITE)" : "") << "\n";
    std::cout << "Biome           : " << session.dungeon().biomeName() << "\n";
    std::cout << "Character level : " << session.player().stats.level << "\n";
    std::cout << "Monsters slain  : " << session.kills() << "\n";
    std::cout << "Gold collected  : " << session.goldEarned() << "\n";
    std::cout << "Guides unlocked : " << session.codex().unlockedCount() << "/" << session.codex().totalGuides() << "\n";
    std::cout << "Lore found      : " << session.codex().loreCount() << "\n";
    std::cout << "Turns taken     : " << session.player().stats.turns << "\n";
    std::cout << "Time played     : " << session.elapsedString() << "\n";
    std::cout << "Seed            : " << session.rng().seedValue() << "\n";
    std::cout << "\nThe darkness closes over the Shadowdeep.\n";
    if (session.depth() > kOriginalMaxDepth) {
        std::cout << "You delved beyond the known depths. Few dare such madness.\n";
    }
}

void App::showWinScreen(const GameSession& session) {
    terminal_->restore();
    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;33mSHADOWDEEP\n\nVICTORY\033[0m\n\n";
    std::cout << "You emerge into daylight carrying the Amulet of Shadowdeep.\n";
    std::cout << "The darkness below falls silent... but you know it stirs deeper.\n\n";
    std::cout << "Character: " << session.player().name << " (" << session.player().className() << ")\n";
    std::cout << "Character level : " << session.player().stats.level << "\n";
    std::cout << "Monsters slain  : " << session.kills() << "\n";
    std::cout << "Gold collected  : " << session.goldEarned() << "\n";
    std::cout << "Guides unlocked : " << session.codex().unlockedCount() << "/" << session.codex().totalGuides() << "\n";
    std::cout << "Lore found      : " << session.codex().loreCount() << "\n";
    std::cout << "Turns taken     : " << session.player().stats.turns << "\n";
    std::cout << "Time played     : " << session.elapsedString() << "\n";
    std::cout << "Seed            : " << session.rng().seedValue() << "\n\n";
    std::cout << "But the void beyond depth 30 calls... will you return for true glory?\n\n";
    std::cout << "Made by github.com/Seigh-sword\n";
    std::cout << "https://github.com/Seigh-sword/shadowdeep\n";
}

int App::runHome() {
    terminal_->enterRaw();
    int selected = 0;
    const int count = 6;

    for (;;) {
        auto sz = terminal_->getSize();
        screen_.resize(sz.cols, sz.rows);
        renderHome(selected);
        blit();

        auto ev = terminal_->waitKey();

        if (ev.code == static_cast<int>(KeyCode::Up) || ev.code == 'w' || ev.code == 'W' || ev.code == 'k') {
            selected = (selected - 1 + count) % count;
        } else if (ev.code == static_cast<int>(KeyCode::Down) || ev.code == 's' || ev.code == 'S' || ev.code == 'j') {
            selected = (selected + 1) % count;
        } else if (ev.code == static_cast<int>(KeyCode::Enter) || ev.code == '\r' || ev.code == '\n') {
            if (selected == 0) {
                auto entries = saveMgr_->listEntries();
                int cursor = 0;
                int scroll = 0;
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderEntries(entries, cursor, scroll);
                    blit();
                    auto ev2 = terminal_->waitKey();

                    if (ev2.code == static_cast<int>(KeyCode::Escape) || ev2.code == 'b' || ev2.code == 'B' || ev2.code == 'q') break;
                    else if (ev2.code == static_cast<int>(KeyCode::Up) || ev2.code == 'k' || ev2.code == 'w') {
                        if (cursor > 0) cursor--;
                        if (cursor < scroll) scroll = cursor;
                    } else if (ev2.code == static_cast<int>(KeyCode::Down) || ev2.code == 'j' || ev2.code == 's') {
                        if (cursor < static_cast<int>(entries.size())) cursor++;
                        if (cursor >= scroll + (screen_.height() - 6) - 1) scroll++;
                    } else if (ev2.code == 'n' || ev2.code == 'N') {
                        std::string entryName = "The Black Road";
                        std::string charName = "Arin";
                        int pclass = 0;
                        uint32_t seed = 0;
                        int field = 0;

                        for (;;) {
                            auto sz3 = terminal_->getSize();
                            screen_.resize(sz3.cols, sz3.rows);
                            renderNewEntryScreen(entryName, charName, pclass, seed, field);
                            blit();
                            auto ev3 = terminal_->waitKey();
                            if (ev3.code == static_cast<int>(KeyCode::Escape)) break;
                            if (ev3.code == static_cast<int>(KeyCode::Up) || ev3.code == 'k' || ev3.code == 'w') field = (field - 1 + 6) % 6;
                            else if (ev3.code == static_cast<int>(KeyCode::Down) || ev3.code == 'j' || ev3.code == 's') field = (field + 1) % 6;
                            else if (ev3.code == static_cast<int>(KeyCode::Enter) || ev3.code == '\r' || ev3.code == '\n') {
                                if (field == 0) {
                                    std::string res = promptString("Entry Name:", entryName);
                                    if (!res.empty()) entryName = res;
                                } else if (field == 1) {
                                    std::string res = promptString("Character Name:", charName);
                                    if (!res.empty()) charName = res;
                                } else if (field == 2) {
                                    pclass = (pclass + 1) % 6;
                                } else if (field == 3) {
                                    std::string res = promptString("Seed (0=random):", std::to_string(seed));
                                    if (!res.empty()) {
                                        try { seed = static_cast<uint32_t>(std::stoul(res)); } catch (...) {}
                                    }
                                } else if (field == 4) {
                                    uint32_t actualSeed = seed == 0 ? static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) : seed;
                                    GameSession session(actualSeed, true);
                                    session.newGame("temp", charName, pclass, actualSeed, "normal");
                                    session.setEntryId("temp");

                                    std::string newId;
                                    {
                                        std::string id;
                                        for (int i = 0; i < 8; ++i) id += static_cast<char>('a' + (rand() % 26));
                                        newId = id + std::to_string(actualSeed);
                                    }

                                    std::filesystem::path p = saveMgr_->entryPath(newId);
                                    std::filesystem::create_directories(p.parent_path());
                                    auto data = session.saveToData();
                                    std::string tmp = p.string() + ".tmp";
                                    {
                                        std::ofstream f(tmp, std::ios::binary);
                                        f.write(reinterpret_cast<const char*>(data.data()), data.size());
                                    }
                                    std::filesystem::rename(std::filesystem::path(tmp), p);

                                    entries = saveMgr_->listEntries();
                                    cursor = 0;
                                    for (size_t i = 0; i < entries.size(); ++i) if (entries[i].entryId == newId) cursor = static_cast<int>(i);
                                    break;
                                } else if (field == 5) {
                                    break;
                                }
                            }
                        }
                    } else if (ev2.code == static_cast<int>(KeyCode::Enter) || ev2.code == '\r' || ev2.code == '\n') {
                        if (cursor < static_cast<int>(entries.size())) {
                            auto& meta = entries[cursor];
                            if (meta.status == EntryStatus::Corrupted) {
                                confirm("Entry corrupted, cannot open");
                                continue;
                            }
                            if (meta.status == EntryStatus::Incompatible) {
                                confirm("Incompatible save - update required");
                                continue;
                            }

                            GameSession session;
                            auto path = saveMgr_->entryPath(meta.entryId);
                            std::ifstream f(path, std::ios::binary);
                            std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

                            uint32_t seed = 0;
                            try { seed = static_cast<uint32_t>(std::stoul(meta.seed)); } catch (...) { seed = static_cast<uint32_t>(rand()); }

                            session = GameSession(seed, true);
                            session.newGame(meta.entryId, meta.characterName, 0, seed, "normal");
                            session.setEntryId(meta.entryId);

                            runGameplay(session);

                            if (session.isDead() || session.isWon()) {
                                saveMgr_->deleteEntry(meta.entryId, true);
                            } else {
                                auto outData = session.saveToData();
                                std::string tmp = path.string() + ".tmp";
                                {
                                    std::ofstream out(tmp, std::ios::binary);
                                    out.write(reinterpret_cast<const char*>(outData.data()), outData.size());
                                }
                                std::filesystem::rename(std::filesystem::path(tmp), path);
                            }

                            entries = saveMgr_->listEntries();
                        } else {
                            std::string entryName = "New Entry";
                            std::string charName = "Hero";
                            int pclass = 0;
                            uint32_t seed = static_cast<uint32_t>(rand());
                            GameSession session(seed, true);
                            session.newGame("temp", charName, pclass, seed, "normal");
                            runGameplay(session);
                        }
                    } else if (ev2.code == 'd' || ev2.code == 'D') {
                        if (cursor < static_cast<int>(entries.size())) {
                            if (confirm("Delete entry " + entries[cursor].entryName + "?")) {
                                saveMgr_->deleteEntry(entries[cursor].entryId, true);
                                entries = saveMgr_->listEntries();
                                if (cursor >= static_cast<int>(entries.size())) cursor = static_cast<int>(entries.size()) - 1;
                                if (cursor < 0) cursor = 0;
                            }
                        }
                    } else if (ev2.code == 'r' || ev2.code == 'R') {
                        if (cursor < static_cast<int>(entries.size())) {
                            std::string newName = promptString("New name:", entries[cursor].entryName);
                            if (!newName.empty()) {
                                entries[cursor].entryName = newName;
                            }
                        }
                    }
                }
            } else if (selected == 1) {
                GameSession dummySession(123, true);
                dummySession.newGame("codex_view", "Viewer", 0, 123, "normal");
                int tab = 0;
                int cur = 0;
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderCodex(dummySession, tab, cur);
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code == static_cast<int>(KeyCode::Escape) || ev2.code == 'q') break;
                    if (ev2.code == '\t' || ev2.code == 'l' || ev2.code == static_cast<int>(KeyCode::Right)) tab = (tab + 1) % 4;
                    if (ev2.code == 'h' || ev2.code == static_cast<int>(KeyCode::Left)) tab = (tab - 1 + 4) % 4;
                    if (ev2.code == static_cast<int>(KeyCode::Up) || ev2.code == 'k' || ev2.code == 'w') { if (cur > 0) cur--; }
                    if (ev2.code == static_cast<int>(KeyCode::Down) || ev2.code == 'j' || ev2.code == 's') cur++;
                }
            } else if (selected == 2) {
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderSettings();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code == static_cast<int>(KeyCode::Escape) || ev2.code == 'q') break;
                }
            } else if (selected == 3) {
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderChangelog();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code != static_cast<int>(KeyCode::None)) break;
                }
            } else if (selected == 4) {
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderCredits();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code != static_cast<int>(KeyCode::None)) break;
                }
            } else if (selected == 5) {
                return 0;
            }
        } else if (ev.code == 'q' || ev.code == 'Q' || ev.code == 4) {
            return 0;
        }
    }
}

int App::runGameplay(GameSession& session) {
    session.start();
    int inventoryCursor = 0;
    bool inInventory = false;
    bool inHelp = false;
    bool inCodex = false;
    int codexTab = 0;
    int codexCursor = 0;

    bool cutsceneShown = false;

    for (;;) {
        auto sz = terminal_->getSize();
        screen_.resize(sz.cols, sz.rows);

        if (!cutsceneShown && session.depth() == 1 && session.player().stats.turns < 2) {
            screen_.clear();
            screen_.drawBox(10, 5, 60, 10, Color::Purple, true);
            screen_.text(12, 6, "CUTSCENE: The Descent Begins", Color::BrightYellow, true);
            std::vector<std::string> lines = {
                "You stand at the mouth of Shadowdeep.",
                "Cold wind howls from below.",
                "Legends speak of the Amulet at depth 30,",
                "but whispers tell of infinite horrors beyond.",
                "",
                "Collect guide fragments ; to learn.",
                "Survive hunger, master biomes.",
                "",
                "Press any key to begin..."
            };
            for (int i = 0; i < static_cast<int>(lines.size()); ++i) screen_.text(12, 8 + i, lines[i], Color::White);
            blit();
            terminal_->waitKey();
            cutsceneShown = true;
            continue;
        }

        if (session.depth() == kOriginalMaxDepth && !cutsceneShown) {
            // depth 30 cutscene
            screen_.clear();
            screen_.drawBox(10, 5, 60, 10, Color::Gold, true);
            screen_.text(12, 6, "CUTSCENE: The Heart of Shadowdeep", Color::Gold, true);
            std::vector<std::string> lines = {
                "You have reached depth 30.",
                "The air thrums with shadow energy.",
                "The Amulet pulses ahead, guarded by ancient evil.",
                "Beyond lies infinite depth - will you dare?",
                "",
                "Press any key..."
            };
            for (int i = 0; i < static_cast<int>(lines.size()); ++i) screen_.text(12, 8 + i, lines[i], Color::White);
            blit();
            terminal_->waitKey();
            cutsceneShown = true;
        }

        if (inHelp) {
            renderHelp();
            blit();
            auto ev = terminal_->waitKey();
            if (ev.code != static_cast<int>(KeyCode::None)) inHelp = false;
            continue;
        }

        if (inCodex) {
            renderCodex(session, codexTab, codexCursor);
            blit();
            auto ev = terminal_->waitKey();
            if (ev.code == static_cast<int>(KeyCode::Escape) || ev.code == 'c' || ev.code == 'C' || ev.code == 'q') { inCodex = false; continue; }
            if (ev.code == '\t' || ev.code == 'l' || ev.code == static_cast<int>(KeyCode::Right)) { codexTab = (codexTab + 1) % 4; codexCursor = 0; }
            else if (ev.code == 'h' || ev.code == static_cast<int>(KeyCode::Left)) { codexTab = (codexTab - 1 + 4) % 4; codexCursor = 0; }
            else if (ev.code == static_cast<int>(KeyCode::Up) || ev.code == 'k' || ev.code == 'w') { if (codexCursor > 0) codexCursor--; }
            else if (ev.code == static_cast<int>(KeyCode::Down) || ev.code == 'j' || ev.code == 's') { codexCursor++; }
            continue;
        }

        if (inInventory) {
            renderGameplay(session);
            renderInventory(session, inventoryCursor);
            blit();

            auto ev = terminal_->waitKey();
            if (ev.code == static_cast<int>(KeyCode::Escape) || ev.code == 'i' || ev.code == 'I' || ev.code == 'q') {
                inInventory = false;
                continue;
            }
            if (ev.code == static_cast<int>(KeyCode::Down) || ev.code == 'j') {
                if (inventoryCursor + 1 < static_cast<int>(session.player().inventory.size())) inventoryCursor++;
            } else if (ev.code == static_cast<int>(KeyCode::Up) || ev.code == 'k') {
                if (inventoryCursor > 0) inventoryCursor--;
            } else if (ev.code == static_cast<int>(KeyCode::Enter) || ev.code == '\r' || ev.code == '\n') {
                if (!session.player().inventory.empty()) {
                    bool used = session.useItem(inventoryCursor);
                    if (used) {
                        inInventory = false;
                        session.endTurn();
                        if (session.isDead()) { showDeathScreen(session); terminal_->waitKey(); return 0; }
                        if (session.isWon()) { showWinScreen(session); terminal_->waitKey(); return 0; }
                    }
                }
            } else if (ev.code == 'x' || ev.code == 'X') {
                if (!session.player().inventory.empty()) {
                    session.dropItem(inventoryCursor);
                    inInventory = false;
                    session.endTurn();
                }
            } else if (ev.code >= 'a' && ev.code <= 'z') {
                int idx = ev.code - 'a';
                if (idx < static_cast<int>(session.player().inventory.size())) {
                    bool used = session.useItem(idx);
                    if (used) {
                        inInventory = false;
                        session.endTurn();
                        if (session.isDead()) { showDeathScreen(session); terminal_->waitKey(); return 0; }
                        if (session.isWon()) { showWinScreen(session); terminal_->waitKey(); return 0; }
                    }
                }
            }
            continue;
        }

        renderGameplay(session);
        blit();

        if (session.isDead()) { showDeathScreen(session); terminal_->waitKey(); return 0; }
        if (session.isWon()) { showWinScreen(session); terminal_->waitKey(); return 0; }

        auto ev = terminal_->waitKey();

        if (ev.code == 'Q' || ev.code == 4) {
            if (confirm("Quit without saving? (y/n)")) return 0;
            else continue;
        }

        if (ev.code == '?') {
            inHelp = true;
            continue;
        }

        if (ev.code == 'i' || ev.code == 'I') {
            inInventory = true;
            inventoryCursor = 0;
            continue;
        }

        if (ev.code == 'c' || ev.code == 'C') {
            inCodex = true;
            codexTab = 0;
            codexCursor = 0;
            continue;
        }

        bool tookTurn = false;
        static int shopCursor = 0;
        static bool showMinimapOverlay = false;

        if (ev.code == 'm' || ev.code == 'M') {
            showMinimapOverlay = !showMinimapOverlay;
            if (showMinimapOverlay) {
                int mw = 40;
                int mh = 20;
                int top = (screen_.height() - mh) / 2;
                int left = (screen_.width() - mw) / 2;
                screen_.drawBox(left, top, mw, mh, Color::BrightCyan, true);
                screen_.text(left + 2, top, " MINIMAP ", Color::BrightYellow, true);
                std::string mini = session.minimapString(mw - 4, mh - 4);
                std::string line;
                int row = 0;
                for (char ch : mini) {
                    if (ch == '\n') {
                        screen_.text(left + 2, top + 1 + row, line.substr(0, mw - 4), Color::White);
                        row++;
                        line.clear();
                        if (row >= mh - 2) break;
                    } else {
                        line += ch;
                    }
                }
                screen_.text(left + 2, top + mh - 1, "Press any key", Color::Gray);
                blit();
                terminal_->waitKey();
            }
            continue;
        }

        if (ev.code == 'o' || ev.code == 'O') {
            if (session.isInShop()) {
                shopCursor = 0;
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    screen_.clear();
                    int w = 70;
                    int h = 20;
                    int top = (screen_.height() - h) / 2;
                    int left = (screen_.width() - w) / 2;
                    screen_.drawBox(left, top, w, h, Color::Gold, true);
                    screen_.text(left + 2, top, " SHOP - Enter to buy, Esc to leave ", Color::BrightYellow, true);
                    std::string goldStr = "Your Gold: " + std::to_string(session.player().stats.gold);
                    screen_.text(left + 2, top + 1, goldStr, Color::Gold);
                    auto& shopItems = session.shopItems();
                    if (shopItems.empty()) {
                        screen_.text(left + 2, top + 3, "(shop empty - you bought everything!)", Color::Gray);
                    } else {
                        int maxRows = h - 6;
                        for (int i = 0; i < static_cast<int>(shopItems.size()) && i < maxRows; ++i) {
                            auto& it = shopItems[i];
                            bool sel = (i == shopCursor);
                            std::string line = (sel ? "> " : "  ") + it.name + " - " + std::to_string(it.valueGold) + "g [" + std::to_string((int)it.rarity) + "]";
                            if (line.size() > (size_t)(w - 4)) line = line.substr(0, w - 4);
                            screen_.text(left + 2, top + 3 + i, line, sel ? Color::BrightWhite : it.color, sel);
                        }
                    }
                    screen_.text(left + 2, top + h - 2, "Up/Down navigate Enter buy Esc leave", Color::Gray);
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code == static_cast<int>(KeyCode::Escape) || ev2.code == 'o' || ev2.code == 'O' || ev2.code == 'q') break;
                    if (ev2.code == static_cast<int>(KeyCode::Up) || ev2.code == 'k') { if (shopCursor > 0) shopCursor--; }
                    else if (ev2.code == static_cast<int>(KeyCode::Down) || ev2.code == 'j') { if (shopCursor + 1 < (int)shopItems.size()) shopCursor++; }
                    else if (ev2.code == static_cast<int>(KeyCode::Enter) || ev2.code == '\r' || ev2.code == '\n') {
                        if (!shopItems.empty()) {
                            bool bought = session.buyItemFromShop(shopCursor);
                            if (bought) {
                                if (shopCursor >= (int)session.shopItems().size()) shopCursor = (int)session.shopItems().size() - 1;
                                if (shopCursor < 0) shopCursor = 0;
                            }
                        }
                    }
                }
                continue;
            } else {
                session.addMessage("No shop here. Find the shop room.", Color::Gray);
                continue;
            }
        }

        if (ev.code == 'a' || ev.code == 'A') {
            session.useAltar();
            tookTurn = true;
        } else if (ev.code == 'f' || ev.code == 'F') {
            session.drinkFountain();
            tookTurn = true;
        } else if (ev.code == static_cast<int>(KeyCode::Up) || ev.code == 'k') tookTurn = session.movePlayer(0, -1);
        else if (ev.code == static_cast<int>(KeyCode::Down) || ev.code == 'j') tookTurn = session.movePlayer(0, 1);
        else if (ev.code == static_cast<int>(KeyCode::Left) || ev.code == 'h') tookTurn = session.movePlayer(-1, 0);
        else if (ev.code == static_cast<int>(KeyCode::Right) || ev.code == 'l') tookTurn = session.movePlayer(1, 0);
        else if (ev.code == 'y') tookTurn = session.movePlayer(-1, -1);
        else if (ev.code == 'u') tookTurn = session.movePlayer(1, -1);
        else if (ev.code == 'b') tookTurn = session.movePlayer(-1, 1);
        else if (ev.code == 'n') tookTurn = session.movePlayer(1, 1);
        else if (ev.code == 'g' || ev.code == ',') tookTurn = session.pickup();
        else if (ev.code == 'q') tookTurn = session.quaffPotion();
        else if (ev.code == 'r') tookTurn = session.readScroll();
        else if (ev.code == '>') { session.descend(); }
        else if (ev.code == '<') { session.ascend(); }
        else if (ev.code == '.' || ev.code == 'z') { tookTurn = session.waitTurn(); }

        if (tookTurn && !session.isDead() && !session.isWon()) {
            session.endTurn();
            if (session.isDead()) { showDeathScreen(session); terminal_->waitKey(); return 0; }
            if (session.isWon()) { showWinScreen(session); terminal_->waitKey(); return 0; }
        }
    }
}

int App::run() {
    if (opts_.showHelp) { printHelp("shadowdeep"); return 0; }
    if (opts_.showVersion) { printVersion(); return 0; }
    if (opts_.showAbout) { printAbout(); return 0; }
    if (opts_.showLicense) { printLicense(); return 0; }

    if (opts_.checkUpdate) {
        std::cout << "Checking for updates...\n";
        std::cout << "Current version: " << kGameVersion << " Build " << kBuildRevision << "\n";
        std::cout << "Platform: " << UpdateManager::getCurrentPlatformString() << " (" << UpdateManager::getOSDisplay() << " " << UpdateManager::getArchDisplay() << ")\n";
        std::cout << "Repository: " << kRepositoryUrl << "\n";
        std::cout << "Direct URL pattern: " << kRepositoryUrl << "/releases/download/(version)/(file)\n";
        std::cout << "API: " << UpdateManager::getLatestReleaseApiUrl() << "\n";

        UpdateManager mgr;
        auto infoOpt = mgr.checkForUpdate(std::string(kGameVersion));
        if (!infoOpt) {
            std::cout << "No update available via API or could not check. Trying direct URL fallback...\n";
            auto direct = mgr.getDirectDownloadUrl(std::string(kGameVersion));
            std::cout << "Direct URL for your platform: " << direct << "\n";
            std::cout << "If that URL exists, you can download manually.\n";
            std::cout << "Check manually: " << UpdateManager::getReleasesPageUrl() << "\n";
            auto cands = UpdateManager::getCandidateArtifactNames();
            std::cout << "Expected artifact names for your platform:\n";
            for (auto& c : cands) {
                std::cout << "  - " << c << "\n";
                std::cout << "    Direct: " << kRepositoryUrl << "/releases/download/" << kGameVersion << "/" << c << "\n";
            }
            return 0;
        }

        auto& info = *infoOpt;
        std::cout << "Update available!\n";
        std::cout << "  Version: " << info.version << " Tag: " << info.tag << "\n";
        std::cout << "  OS/Arch: " << info.os << "/" << info.arch << "\n";
        std::cout << "  Artifact: " << info.artifactName << "\n";
        std::cout << "  URL: " << info.downloadUrl << "\n";
        std::cout << "  Direct fallback: " << kRepositoryUrl << "/releases/download/" << info.version << "/" << info.artifactName << "\n";
        std::cout << "  Size: " << info.size << " bytes\n";
        if (!info.notes.empty()) {
            std::cout << "  Notes: " << info.notes.substr(0, 500) << "\n";
        }
        std::cout << "Run with --update to download.\n";
        return 0;
    }

    if (opts_.doUpdate || opts_.autoUpdate) {
        bool autoInstall = opts_.autoUpdate;
        std::cout << (autoInstall ? "Auto-update requested.\n" : "Update requested.\n");
        std::cout << "Platform: " << UpdateManager::getCurrentPlatformString() << "\n";
        std::cout << "Direct URL pattern: https://github.com/Seigh-sword/shadowdeep/releases/download/(version)/(file)\n";
        std::cout << "Example: https://github.com/Seigh-sword/shadowdeep/releases/download/Zv1/shadowdeep-Zv1-linux-x86_64.tar.gz\n";
        UpdateManager mgr;
        auto infoOpt = mgr.checkForUpdate(std::string(kGameVersion));
        if (!infoOpt) {
            std::cout << "No update found via API, trying direct pattern for latest...\n";
            std::string direct = mgr.getDirectDownloadUrl(std::string(kGameVersion));
            std::cout << "Try manual download: " << direct << "\n";
            std::cout << "Or visit: https://github.com/Seigh-sword/shadowdeep/releases\n";
            auto cands = UpdateManager::getCandidateArtifactNames();
            for (auto& c : cands) {
                std::cout << "  Direct: https://github.com/Seigh-sword/shadowdeep/releases/download/" << kGameVersion << "/" << c << "\n";
            }
            return 0;
        }
        auto& info = *infoOpt;
        std::cout << "Downloading " << info.artifactName << " from " << info.downloadUrl << "\n";
        std::cout << "Direct fallback URL: https://github.com/Seigh-sword/shadowdeep/releases/download/" << info.version << "/" << info.artifactName << "\n";

        std::string destDir;
        try {
            auto paths = getAppPaths();
            destDir = (paths.cacheDir / "updates").string();
            std::filesystem::create_directories(destDir);
        } catch (...) {
            destDir = ".";
        }
        std::string destPath = destDir + "/" + info.artifactName;

        std::cout << "Saving to: " << destPath << "\n";

        if (!mgr.downloadUpdate(info, destPath)) {
            std::cout << "API download failed, trying direct URL...\n";
            std::string directUrl = std::string(kRepositoryUrl) + "/releases/download/" + info.version + "/" + info.artifactName;
            UpdateInfo directInfo = info;
            directInfo.downloadUrl = directUrl;
            if (!mgr.downloadUpdate(directInfo, destPath)) {
                std::cout << "Download failed. Try manual download from: " << info.downloadUrl << "\n";
                std::cout << "Or direct: " << directUrl << "\n";
                std::cout << "All direct URLs for this version:\n";
                for (auto& cand : UpdateManager::getCandidateArtifactNames()) {
                    std::cout << "  https://github.com/Seigh-sword/shadowdeep/releases/download/" << info.version << "/" << cand << "\n";
                }
                return 1;
            }
        }

        std::cout << "Download complete, verifying...\n";

        if (!mgr.verifySize(destPath, info.size)) {
            std::cout << "Size verification warning: Expected " << info.size << " but file size differs - may still be OK if direct URL used.\n";
        }

        if (!info.sha256.empty()) {
            if (!mgr.verifyChecksum(destPath, info.sha256)) {
                std::cout << "Checksum verification failed.\n";
                return 1;
            }
            std::cout << "Checksum OK.\n";
        } else {
            std::cout << "No checksum provided, size check done.\n";
        }

        if (autoInstall) {
            std::cout << "Attempting auto-install...\n";
            std::cout << "Current exe: " << UpdateManager::getCurrentExecutablePath() << "\n";
            if (mgr.attemptAutoInstall(destPath, info)) {
                std::cout << "Auto-install succeeded! Restart the game.\n";
                return 0;
            } else {
                std::cout << "Auto-install not possible (permissions or platform). Falling back to staged.\n";
            }
        }

        std::cout << "Update staged at: " << destPath << "\n";
        std::cout << "To install:\n";
        std::cout << "  - On Linux/macOS/BSD: tar xzf " << destPath << " and replace binary\n";
        std::cout << "  - On Windows: unzip " << destPath << " and replace .exe (close game first, staged updater will handle self-replace)\n";
        std::cout << "Your saves are never overwritten, they are in separate user data directory.\n";
        std::cout << "Direct URLs are predictable: https://github.com/Seigh-sword/shadowdeep/releases/download/(version)/(file)\n";
        std::cout << "See DOCS/BUILDING.md for install instructions.\n";
        return 0;
    }

    if (!init()) {
        std::cerr << "Failed to initialize\n";
        return 1;
    }

    if (opts_.seed) {
        GameSession session(*opts_.seed, true);
        session.newGame("direct", "Hero", 0, *opts_.seed, "normal");
        int res = runGameplay(session);
        shutdown();
        return res;
    }

    int res = runHome();
    shutdown();
    return res;
}

}
