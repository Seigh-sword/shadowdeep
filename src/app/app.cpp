#include "shadowdeep/app/app.hpp"
#include "shadowdeep/version.hpp"
#include "shadowdeep/world/tile.hpp"
#include "shadowdeep/entities/player.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <cstdio>

namespace shadowdeep {

App::App(CliOptions opts) : opts_(std::move(opts)), screen_(80, 30) {}

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
    auto s = screen_.toAnsi(false);
    terminal_->writeRaw(s);
}

std::string App::promptString(const std::string& prompt, const std::string& initial) {
    std::string cur = initial;
    int cursor = static_cast<int>(cur.size());

    for (;;) {
        screen_.clear();
        screen_.drawBox(10, 5, 60, 7, Color::BrightCyan);
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
        screen_.drawBox(15, 8, 50, 6, Color::BrightRed);
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

    std::string title = "S H A D O W D E E P";
    int tx = (w - static_cast<int>(title.size())) / 2;
    screen_.text(tx, 2, title, Color::BrightYellow, true);

    std::string ver = std::string(kGameVersion);
    int vx = (w - static_cast<int>(ver.size())) / 2;
    screen_.text(vx, 3, ver, Color::BrightCyan, true);

    int boxW = 27;
    int boxH = 9;
    int bx = (w - boxW) / 2;
    int by = 6;

    screen_.drawBox(bx, by, boxW, boxH, Color::White);

    std::vector<std::string> items = {"Entries", "Settings", "Changelog", "Credits", "Quit"};
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        int y = by + 1 + i;
        int x = bx + 2;
        bool sel = (i == selected);
        std::string prefix = sel ? "> " : "  ";
        Color c = sel ? Color::BrightWhite : Color::White;
        screen_.text(x, y, prefix + items[i], c, sel);
    }

    screen_.text(2, h - 2, "Up/Down W/S navigate  Enter select  Q quit", Color::Gray);

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
        screen_.text(2, 3, "(no entries)", Color::Gray);
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
            line += "  Lv" + std::to_string(e.level);
            line += "  " + e.regionName.substr(0, 12);
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
        "Seed: " + std::to_string(seed) + " (0=random)",
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

    std::string title = "SHADOWDEEP " + std::string(kGameVersion) + " Depth " + std::to_string(session.depth()) + "/" + std::to_string(kMaxDepth) + " " + regionNameForDepth(session.depth()) + " Seed " + std::to_string(session.rng().seedValue());
    screen_.text(0, 0, title.substr(0, w), Color::BrightCyan, true);

    auto& p = session.player();
    std::string status = "HP " + std::to_string(p.stats.hp) + "/" + std::to_string(p.stats.maxHp) + " Lv " + std::to_string(p.stats.level) + " XP " + std::to_string(p.stats.xp) + "/" + std::to_string(p.stats.xpNext) + " Atk " + std::to_string(p.attackPower()) + " Def " + std::to_string(p.defensePower()) + " Gold " + std::to_string(p.stats.gold) + " Rubies " + std::to_string(p.stats.rubies);
    if (p.hasteTurns > 0) status += " Haste " + std::to_string(p.hasteTurns);
    if (!p.effects.all().empty()) {
        status += " [";
        for (auto& e : p.effects.all()) status += e.displayName() + " ";
        status += "]";
    }
    screen_.text(0, 1, status.substr(0, w), p.stats.hp <= p.stats.maxHp / 3 ? Color::BrightRed : Color::BrightWhite, true);

    int mapTop = 3;
    int mapLeft = 2;
    int mapW = kMapW;
    int mapH = kMapH;

    if (w < mapW + 10 || h < mapH + 10) {
        screen_.text(2, 4, "Terminal too small. Please resize to at least 80x33.", Color::BrightRed, true);
        std::string sz = "Current: " + std::to_string(w) + "x" + std::to_string(h);
        screen_.text(2, 5, sz, Color::White);
        return;
    }

    auto& dungeon = session.dungeon();

    for (int y = 0; y < mapH; ++y) {
        for (int x = 0; x < mapW; ++x) {
            Vec2 pp{x, y};
            if (!dungeon.explored[y][x]) continue;
            bool vis = dungeon.visible[y][x];
            Tile t = dungeon.at(pp);
            char ch = tileGlyph(t);
            Color c = vis ? Color::Gray : Color::Blue;
            switch (t) {
                case Tile::Wall: c = vis ? Color::Steel : Color::Blue; break;
                case Tile::DoorClosed: c = vis ? Color::Brown : Color::Blue; break;
                case Tile::DoorOpen: c = vis ? Color::Brown : Color::Blue; break;
                case Tile::StairsDown: case Tile::StairsUp: c = vis ? Color::BrightYellow : Color::Blue; break;
                case Tile::Rubble: c = vis ? Color::Red : Color::Blue; break;
                case Tile::Water: c = vis ? Color::Blue : Color::Blue; break;
                case Tile::Lava: c = vis ? Color::BrightRed : Color::Blue; break;
                default: break;
            }
            screen_.put(mapLeft + x, mapTop + y, ch, c, vis);
        }
    }

    for (auto& it : session.items()) {
        if (!dungeon.visible[it.pos.y][it.pos.x]) continue;
        screen_.put(mapLeft + it.pos.x, mapTop + it.pos.y, it.glyph, it.color, true);
    }

    for (auto& m : session.monsters()) {
        if (!m.alive) continue;
        if (!dungeon.visible[m.pos.y][m.pos.x]) continue;
        screen_.put(mapLeft + m.pos.x, mapTop + m.pos.y, m.glyph, m.color, true);
    }

    screen_.put(mapLeft + p.pos.x, mapTop + p.pos.y, '@', Color::BrightWhite, true);

    int msgTop = mapTop + mapH + 1;
    int msgCount = std::min(kMsgLines, static_cast<int>(session.messages().size()));
    int start = static_cast<int>(session.messages().size()) - msgCount;
    for (int i = 0; i < msgCount; ++i) {
        auto& msg = session.messages()[start + i];
        std::string txt = msg.text;
        if (msg.count > 1) txt += " (x" + std::to_string(msg.count) + ")";
        if (txt.size() > static_cast<size_t>(w - 2)) txt = txt.substr(0, w - 2);
        screen_.text(2, msgTop + i, txt, msg.color);
    }

    screen_.text(2, h - 2, "move hjkl/yubn/arrows g:get i:inv q:quaff r:read </>:stairs z:wait ?:help Q:quit", Color::Gray);
}

void App::renderInventory(GameSession& session, int cursor) {
    int w = 60;
    int h = std::min(screen_.height() - 2, std::max(14, static_cast<int>(session.player().inventory.size()) + 10));
    int top = (screen_.height() - h) / 2;
    int left = (screen_.width() - w) / 2;

    screen_.drawBox(left, top, w, h, Color::BrightCyan);
    screen_.text(left + 2, top, " INVENTORY ", Color::BrightYellow, true);

    auto& inv = session.player().inventory.all();

    if (inv.empty()) {
        screen_.text(left + 3, top + 2, "(empty)", Color::Gray);
    } else {
        int maxRows = h - 7;
        for (int i = 0; i < static_cast<int>(inv.size()) && i < maxRows; ++i) {
            auto& it = inv[i];
            std::string s = (i == cursor ? "> " : "  ");
            s += static_cast<char>('a' + i);
            s += ") ";
            s += it.fullName();
            if (s.size() > static_cast<size_t>(w - 4)) s = s.substr(0, w - 4);
            screen_.text(left + 2, top + 2 + i, s, it.color, i == cursor);
        }
    }

    auto& eq = session.player().equipment;
    std::string weapon = "Weapon: ";
    weapon += eq.mainHand ? eq.mainHand->fullName() : "bare fists";
    screen_.text(left + 2, top + h - 4, weapon.substr(0, w - 4), Color::Steel);

    std::string armor = "Armour: ";
    armor += eq.body ? eq.body->fullName() : "no armour";
    screen_.text(left + 2, top + h - 3, armor.substr(0, w - 4), Color::Steel);

    screen_.text(left + 2, top + h - 2, "Enter/use x/drop i/Esc close", Color::Gray);
}

void App::renderHelp() {
    screen_.clear();
    int w = 66;
    int h = 24;
    int top = (screen_.height() - h) / 2;
    int left = (screen_.width() - w) / 2;

    screen_.drawBox(left, top, w, h, Color::BrightYellow);
    std::vector<std::string> lines = {
        "SHADOWDEEP " + std::string(kGameVersion),
        "",
        "Arrow keys / hjkl / yubn    Move or melee attack",
        "g or ,                      Pick up item",
        "i                           Inventory",
        "q                           Quaff first potion",
        "r                           Read first scroll",
        ">                           Descend stairs",
        "<                           Ascend stairs",
        "z or .                      Wait",
        "?                           Help",
        "Q                           Quit",
        "",
        "@ you       letters monsters       ! potion",
        "? scroll    / weapon    [ armour    $ gold/rubies",
        "% food      \" Amulet    + closed door",
        "",
        "Goal: reach depth 30 and recover the Amulet.",
        "Then climb back to depth 1 and escape.",
        "",
        "Press any key to return."
    };

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        Color c = (i == 0) ? Color::BrightYellow : Color::White;
        bool bold = (i == 0);
        screen_.text(left + 2, top + 1 + i, lines[i].substr(0, w - 4), c, bold);
    }
}

void App::renderSettings() {
    screen_.clear();
    int w = screen_.width();
    screen_.text(2, 0, "Settings", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    auto& cfg = configMgr_->get();
    std::vector<std::string> lines = {
        "Theme: " + cfg.theme,
        "Animations: " + std::string(cfg.animations ? "On" : "Off"),
        "Reduced Motion: " + std::string(cfg.reducedMotion ? "On" : "Off"),
        "Screen Shake: " + std::string(cfg.screenShake ? "On" : "Off"),
        "High Contrast: " + std::string(cfg.highContrast ? "On" : "Off"),
        "Mouse Support: " + std::string(cfg.mouseSupport ? "On" : "Off"),
        "Auto Pickup Gold: " + std::string(cfg.autoPickupGold ? "On" : "Off"),
        "Auto Check Update: " + std::string(cfg.autoCheckUpdate ? "On" : "Off"),
        "Update Channel: " + cfg.updateChannel,
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
    screen_.text(2, 0, "Changelog - " + std::string(kGameVersion) + " Early Alpha", Color::BrightYellow, true);
    screen_.drawHLine(0, 1, w, "-", Color::Gray);

    std::vector<std::string> lines = {
        "Zv1 Early Alpha - In Development",
        "",
        "Added:",
        "  - Multi-file C++20 architecture",
        "  - Cross-platform terminal backend (POSIX + Windows)",
        "  - Platform paths (XDG, AppData, Application Support)",
        "  - Binary save format with atomic saving and backups",
        "  - Entries management (New, Open, Rename, Delete, Duplicate, Backup, Export, Import)",
        "  - Home screen with responsive layout",
        "  - Settings, Changelog, Credits screens",
        "  - Expanded dungeon generation (rooms, BSP, cavern, features)",
        "  - 30 depths across 10 thematic regions",
        "  - Expanded monsters (30+ types) with distinct AI",
        "  - Status effects system",
        "  - Item rarity and enchantments",
        "  - Gold and Rubies economy",
        "  - Improved combat with damage types",
        "  - CLI: --version --about --license --help --seed --check-update --update",
        "",
        "Changed:",
        "  - From single-file to modular project",
        "  - Terminal handling now abstracted",
        "  - Rendering now uses virtual screen buffer",
        "",
        "Fixed:",
        "  - POSIX-only headers causing Windows build failure",
        "  - Various gameplay bugs from original",
        "",
        "Known Issues:",
        "  - Balancing ongoing",
        "  - Some regions still share generation",
        "",
        "Press any key to return"
    };

    for (int i = 0; i < static_cast<int>(lines.size()) && i < screen_.height() - 4; ++i) {
        Color c = Color::White;
        if (lines[i].rfind("Added:", 0) == 0 || lines[i].rfind("Changed:", 0) == 0 || lines[i].rfind("Fixed:", 0) == 0 || lines[i].rfind("Known", 0) == 0) c = Color::BrightCyan;
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
        "SHADOWDEEP",
        "A roguelike dungeon crawler written in C++",
        "",
        "Made by:",
        "github.com/Seigh-sword",
        "",
        "Repository:",
        "https://github.com/Seigh-sword/shadowdeep",
        "",
        "Licensed under the ISC License.",
        "",
        "Version: " + std::string(kGameVersion) + " (" + std::string(kVersionHuman) + ")",
        "Save schema: " + std::to_string(kSaveSchemaVersion),
        "Config schema: " + std::to_string(kConfigSchemaVersion),
        "Build revision: " + std::to_string(kBuildRevision),
        "",
        "Third-party:",
        "  - fmt (MIT)",
        "  - spdlog (MIT) - optional",
        "  - nlohmann/json (MIT) - config",
        "  - FTXUI (MIT) - TUI (optional backend)",
        "  - cpr/libcurl (MIT) - updates",
        "  - Catch2 (BSL-1.0) - tests",
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
        if (it->count > 1) txt += " (x" + std::to_string(it->count) + ")";
        screen_.text(2, y++, txt.substr(0, screen_.width() - 4), it->color);
    }

    screen_.text(2, screen_.height() - 2, "Press any key to return", Color::Gray);
}

void App::showDeathScreen(const GameSession& session) {
    terminal_->restore();
    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;31mSHADOWDEEP\n\nYOU HAVE DIED\033[0m\n\n";
    std::cout << "Character: " << session.player().name << " (" << session.player().className() << ")\n";
    std::cout << "Depth reached   : " << session.depth() << "\n";
    std::cout << "Character level : " << session.player().stats.level << "\n";
    std::cout << "Monsters slain  : " << session.kills() << "\n";
    std::cout << "Gold collected  : " << session.goldEarned() << "\n";
    std::cout << "Turns taken     : " << session.player().stats.turns << "\n";
    std::cout << "Time played     : " << session.elapsedString() << "\n";
    std::cout << "Seed            : " << session.rng().seedValue() << "\n";
    std::cout << "\nThe darkness closes over the Shadowdeep.\n";
}

void App::showWinScreen(const GameSession& session) {
    terminal_->restore();
    std::cout << "\033[2J\033[H";
    std::cout << "\033[1;33mSHADOWDEEP\n\nVICTORY\033[0m\n\n";
    std::cout << "You emerge into daylight carrying the Amulet of Shadowdeep.\n";
    std::cout << "The darkness below falls silent.\n\n";
    std::cout << "Character: " << session.player().name << " (" << session.player().className() << ")\n";
    std::cout << "Character level : " << session.player().stats.level << "\n";
    std::cout << "Monsters slain  : " << session.kills() << "\n";
    std::cout << "Gold collected  : " << session.goldEarned() << "\n";
    std::cout << "Turns taken     : " << session.player().stats.turns << "\n";
    std::cout << "Time played     : " << session.elapsedString() << "\n";
    std::cout << "Seed            : " << session.rng().seedValue() << "\n\n";
    std::cout << "Made by github.com/Seigh-sword\n";
    std::cout << "https://github.com/Seigh-sword/shadowdeep\n";
}

int App::runHome() {
    terminal_->enterRaw();
    int selected = 0;
    const int count = 5;

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
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderSettings();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code == static_cast<int>(KeyCode::Escape) || ev2.code == 'q') break;
                }
            } else if (selected == 2) {
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderChangelog();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code != static_cast<int>(KeyCode::None)) break;
                }
            } else if (selected == 3) {
                for (;;) {
                    auto sz2 = terminal_->getSize();
                    screen_.resize(sz2.cols, sz2.rows);
                    renderCredits();
                    blit();
                    auto ev2 = terminal_->waitKey();
                    if (ev2.code != static_cast<int>(KeyCode::None)) break;
                }
            } else if (selected == 4) {
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

    for (;;) {
        auto sz = terminal_->getSize();
        screen_.resize(sz.cols, sz.rows);

        if (inHelp) {
            renderHelp();
            blit();
            auto ev = terminal_->waitKey();
            if (ev.code != static_cast<int>(KeyCode::None)) inHelp = false;
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

        if (ev.code == '?' ) {
            inHelp = true;
            continue;
        }

        if (ev.code == 'i' || ev.code == 'I') {
            inInventory = true;
            inventoryCursor = 0;
            continue;
        }

        bool tookTurn = false;

        if (ev.code == static_cast<int>(KeyCode::Up) || ev.code == 'k') tookTurn = session.movePlayer(0, -1);
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
        std::cout << "Current version: " << kGameVersion << "\n";
        std::cout << "Repository: " << kRepositoryUrl << "\n";
        std::cout << "No update mechanism configured yet, please check GitHub Releases.\n";
        return 0;
    }

    if (opts_.doUpdate) {
        std::cout << "Update requested.\n";
        std::cout << "Please download latest release from: " << kRepositoryUrl << "/releases\n";
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
