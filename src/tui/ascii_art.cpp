#include "shadowdeep/tui/ascii_art.hpp"
#include <algorithm>
#include <cmath>

namespace shadowdeep {

AsciiArtLibrary& AsciiArtLibrary::instance() {
    static AsciiArtLibrary lib;
    return lib;
}

AsciiArtLibrary::AsciiArtLibrary() {
    initMonsters();
    initItems();
    initTiles();
    initBanners();
}

void AsciiArtLibrary::initMonsters() {
    monsters_["monster.giant_rat"] = AsciiSprite({
        "  /\\_/\\",
        " ( o.o)",
        "  > ^ <"
    }, Color::Brown, Color::Default, false);

    monsters_["monster.cave_bat"] = AsciiSprite({
        " /\\   /\\",
        "(  \\_/  )",
        " \\     /",
        "  `---'"
    }, Color::Gray, Color::Default, false);

    monsters_["monster.kobold"] = AsciiSprite({
        "  ,--.",
        " ( o_o)",
        " /  |  \\",
        "  /_\\_\\"
    }, Color::Yellow, Color::Default, false);

    monsters_["monster.goblin"] = AsciiSprite({
        "  .--.",
        " ( o o)",
        " (  V )",
        "  \\ | /",
        "   \\|/"
    }, Color::Green, Color::Default, false);

    monsters_["monster.goblin_archer"] = AsciiSprite({
        "  .--.  /}",
        " ( o o)/",
        " (  V )",
        "  \\ | /"
    }, Color::BrightGreen, Color::Default, false);

    monsters_["monster.goblin_shaman"] = AsciiSprite({
        "  .--. *",
        " ( o o)",
        " (  V )~",
        "  /_I_\\"
    }, Color::BrightMagenta, Color::Default, true);

    monsters_["monster.giant_spider"] = AsciiSprite({
        " /\\  /\\",
        "(--\\/--)",
        " \\_||_/",
        "  /  \\"
    }, Color::Magenta, Color::Default, false);

    monsters_["monster.skeleton"] = AsciiSprite({
        "  .--.",
        " ( o o)",
        "  ( V )",
        "  / | \\",
        "  /_\\_\\"
    }, Color::BrightWhite, Color::Default, false);

    monsters_["monster.zombie"] = AsciiSprite({
        "  .--.",
        " ( - -)",
        " (  ~ )",
        "  /_\\_\\",
        "   / \\"
    }, Color::Green, Color::Default, false);

    monsters_["monster.orc"] = AsciiSprite({
        "  ,---.",
        " ( O O )",
        " (  ^  )",
        "  \\_-_/",
        "   /|\\"
    }, Color::Green, Color::Default, true);

    monsters_["monster.ogre"] = AsciiSprite({
        "   .---.",
        "  ( O O )",
        "  (  ^  )",
        "  /_\\ /_\\",
        "   /_V_\\"
    }, Color::Brown, Color::Default, true);

    monsters_["monster.wraith"] = AsciiSprite({
        "  .---.",
        " /  o  \\",
        "|  \\_/  |",
        " \\     /",
        "  `---'"
    }, Color::BrightCyan, Color::Default, false);

    monsters_["monster.vampire"] = AsciiSprite({
        "  .--.",
        " ( v v)",
        " (  V  )",
        "  \\ | /",
        "   /_\\"
    }, Color::BrightRed, Color::Default, true);

    monsters_["monster.troll"] = AsciiSprite({
        "   .----.",
        "  ( o  o )",
        "  (  -- )",
        "   /|\\/|\\",
        "  /_\\  /_\\"
    }, Color::Green, Color::Default, true);

    monsters_["monster.slime"] = AsciiSprite({
        "  .----.",
        " /      \\",
        "|  o  o |",
        " \\_----_/"
    }, Color::Green, Color::Default, false);

    monsters_["monster.mimic"] = AsciiSprite({
        "  .----.",
        " | o  o |",
        " |  __  |",
        " |_/  \\_|"
    }, Color::Brown, Color::Default, false);

    monsters_["monster.chest_mimic"] = AsciiSprite({
        "  .====.",
        " | o  o |",
        " |  __  |",
        " |_/VV\\_|"
    }, Color::Gold, Color::Default, true);

    monsters_["monster.shopkeeper"] = AsciiSprite({
        "   .--.",
        "  ( o o)",
        "  (  ^ )",
        "  /_\\|/_\\",
        "   /_ _\\"
    }, Color::BrightYellow, Color::Default, true);

    monsters_["monster.shop_guard"] = AsciiSprite({
        "   .--.",
        "  ( O O)",
        "  /  ^  \\",
        " |  [_] |",
        "  \\_\\_/_/"
    }, Color::Steel, Color::Default, true);

    monsters_["monster.cultist"] = AsciiSprite({
        "   .--.",
        "  ( o o)",
        "   ( V )",
        "  / /_\\ \\",
        "   /_\\/_\\"
    }, Color::Purple, Color::Default, false);

    monsters_["monster.elemental_fire"] = AsciiSprite({
        "   (  )",
        "  (    )",
        " (  /\\  )",
        "  ( \\/ )",
        "   (  )"
    }, Color::BrightRed, Color::Default, true);

    monsters_["monster.golem"] = AsciiSprite({
        "  .-----.",
        " | o   o |",
        " |   ^   |",
        " |  \\_/  |",
        "  \\_____/"
    }, Color::Steel, Color::Default, true);

    monsters_["monster.dark_knight"] = AsciiSprite({
        "   .---.",
        "  /  o  \\",
        " |  /_\\  |",
        " |  [_]  |",
        "  \\_\\ /_/"
    }, Color::BrightWhite, Color::Default, true);

    monsters_["monster.lich"] = AsciiSprite({
        "   .---.",
        "  ( o o )",
        "  (  V  )",
        "  /_\\|/_\\",
        "   /_ _\\  *"
    }, Color::BrightMagenta, Color::Default, true);

    monsters_["monster.hydra"] = AsciiSprite({
        "  /\\ /\\ /\\",
        " ( o o o )",
        "  \\_\\_/_/",
        "   /_V_\\"
    }, Color::Green, Color::Default, true);

    monsters_["monster.demon"] = AsciiSprite({
        "  /\\   /\\",
        " (  o o  )",
        " (   V   )",
        "  \\  ^  /",
        "   \\_/_/"
    }, Color::BrightRed, Color::Default, true);

    monsters_["monster.dragon_young"] = AsciiSprite({
        "      /\\",
        "  /\\ /  \\",
        " (  o o  )",
        "  \\  ^  /",
        "   \\_/_/\\"
    }, Color::BrightYellow, Color::Default, true);

    monsters_["monster.ancient_dragon"] = AsciiSprite({
        "          /\\",
        "     /\\  /  \\  /\\",
        "    (  \\/ o o \\/  )",
        "     \\   \\  ^  /   /",
        "      \\__ \\_/_/ __/",
        "         \\_\\_/_/"
    }, Color::Gold, Color::Default, true);

    monsters_["monster.goblin_king"] = AsciiSprite({
        "   .--.--.",
        "  ( O  O )",
        "  (  __  )",
        "   /_\\/_\\",
        "   /_\\/_\\  ++"
    }, Color::Gold, Color::Default, true);

    monsters_["monster.lich_king"] = AsciiSprite({
        "    .-----.",
        "   / o   o \\",
        "  |    V    |",
        "  |  \\___/  |",
        "   \\_\\| |/_/",
        "     /_ _\\  **"
    }, Color::Gold, Color::Default, true);

    monsters_["monster.demon_lord"] = AsciiSprite({
        "   /\\     /\\",
        "  (  \\   /  )",
        "  ( o  V  o )",
        "   \\   ^   /",
        "    \\_\\_/_/",
        "     /_V_\\"
    }, Color::BrightRed, Color::Default, true);

    monsters_["monster.shadow_lord"] = AsciiSprite({
        "   .-----.",
        "  /  o o  \\",
        " |    V    |",
        " |  \\___/  |",
        "  \\  /_\\  /",
        "   \\/_ _\\/"
    }, Color::Purple, Color::Default, true);

    monsters_["monster.void_horror"] = AsciiSprite({
        "   .-----.",
        "  / .   . \\",
        " |   \\_/   |",
        " |  /   \\  |",
        "  \\/____\\/",
        "   /_/ \\_\\"
    }, Color::Purple, Color::Default, true);

    monsters_["monster.titan"] = AsciiSprite({
        "    .-----.",
        "   /  O O  \\",
        "  |    ^    |",
        "  |   \\_/   |",
        "   \\___|___/",
        "    /_/ \\_\\",
        "   /_/   \\_\\"
    }, Color::BrightWhite, Color::Default, true);

    monsters_["monster.dragon_elder"] = AsciiSprite({
        "           /\\",
        "      /\\  /  \\   /\\",
        "     /  \\/ o o \\/  \\",
        "    (    \\  ^  /    )",
        "     \\__  \\_/_/  __/",
        "        \\_\\_/_/_/",
        "         /_V_\\"
    }, Color::Gold, Color::Default, true);

    monsters_["monster.wolf"] = AsciiSprite({
        "  /\\   /\\",
        " (  o o )",
        "  \\  ^  /",
        "   /_\\_\\"
    }, Color::Gray, Color::Default, false);

    monsters_["monster.bear"] = AsciiSprite({
        "  /\\   /\\",
        " ( o   o )",
        " (   ^   )",
        "  \\_\\_/_/"
    }, Color::Brown, Color::Default, true);

    monsters_["monster.harpy"] = AsciiSprite({
        "  /\\   /\\",
        " (  o o  )",
        "  \\  V  /",
        "   /_\\_\\  /\\"
    }, Color::Gray, Color::Default, false);

    monsters_["monster.bandit"] = AsciiSprite({
        "  .--.",
        " ( o o)",
        " (  - )",
        "  /_\\_\\"
    }, Color::Yellow, Color::Default, false);

    monsters_["monster.assassin"] = AsciiSprite({
        "  .--.",
        " ( - -)",
        " (  V )",
        "  /_\\_\\  /"
    }, Color::Gray, Color::Default, false);

    monsters_["monster.spectre"] = AsciiSprite({
        "  .---.",
        " (  o  )",
        "  \\ | /",
        "   \\|/"
    }, Color::BrightCyan, Color::Default, false);

    monsters_["monster.imp"] = AsciiSprite({
        " /\\   /\\",
        "(  o o )",
        " \\  ^  /",
        "  \\_/_/"
    }, Color::BrightRed, Color::Default, false);

    monsters_["monster.balor"] = AsciiSprite({
        "  /\\   /\\",
        " ( o   o )",
        " (   V   )",
        "  \\  _  /",
        "   \\_/_/  !!"
    }, Color::BrightRed, Color::Default, true);
}

void AsciiArtLibrary::initItems() {
    items_["item.dagger"] = AsciiSprite({
        "  /",
        " /",
        "/"
    }, Color::Steel, Color::Default, false);

    items_["item.short_sword"] = AsciiSprite({
        "   /",
        "  /",
        " /",
        "/"
    }, Color::Steel, Color::Default, false);

    items_["item.longsword"] = AsciiSprite({
        "    /|",
        "   / |",
        "  /  |",
        " /   |",
        "/____|"
    }, Color::Steel, Color::Default, true);

    items_["item.battle_axe"] = AsciiSprite({
        "  .--.",
        " /    \\",
        "|      |",
        " \\____/",
        "   ||"
    }, Color::Steel, Color::Default, true);

    items_["item.longbow"] = AsciiSprite({
        "  /}",
        " / }",
        "(  }",
        " \\ }",
        "  \\}"
    }, Color::Brown, Color::Default, false);

    items_["item.leather_armour"] = AsciiSprite({
        " .----.",
        "|      |",
        "|  []  |",
        " \\____/"
    }, Color::Steel, Color::Default, false);

    items_["item.plate_armour"] = AsciiSprite({
        " .====.",
        "| [][] |",
        "| [][] |",
        " \\====/"
    }, Color::Steel, Color::Default, true);

    items_["item.buckler"] = AsciiSprite({
        " .---.",
        "(  O  )",
        " `---'"
    }, Color::Steel, Color::Default, false);

    items_["item.helmet"] = AsciiSprite({
        " .---.",
        "/  o  \\",
        "|_____|"
    }, Color::Steel, Color::Default, false);

    items_["item.ring_haste"] = AsciiSprite({
        " .---.",
        "(  o  )",
        " `---'"
    }, Color::Gold, Color::Default, false);

    items_["item.amulet_protection"] = AsciiSprite({
        "  .--.",
        " (    )",
        "  `--'"
    }, Color::Gold, Color::Default, false);

    items_["item.amulet_shadowdeep"] = AsciiSprite({
        "   .--.",
        "  ( ** )",
        "   `--'",
        "   /\\/\\"
    }, Color::Gold, Color::Default, true);

    items_["item.potion_heal"] = AsciiSprite({
        "  .--.",
        " |    |",
        " | ++ |",
        " |____|"
    }, Color::BrightRed, Color::Default, false);

    items_["item.potion_strength"] = AsciiSprite({
        "  .--.",
        " |    |",
        " | /\\ |",
        " |____|"
    }, Color::BrightMagenta, Color::Default, false);

    items_["item.food"] = AsciiSprite({
        " .----.",
        "| o  o |",
        " `----'"
    }, Color::Brown, Color::Default, false);

    items_["item.bread"] = AsciiSprite({
        " .----.",
        "(      )",
        " `----'"
    }, Color::Brown, Color::Default, false);

    items_["item.scroll_lightning"] = AsciiSprite({
        " .----.",
        "| /\\  |",
        "| \\/  |",
        " `----'"
    }, Color::BrightYellow, Color::Default, false);

    items_["item.scroll_fireball"] = AsciiSprite({
        " .----.",
        "| (  ) |",
        "|  \\/  |",
        " `----'"
    }, Color::BrightRed, Color::Default, false);

    items_["item.scroll_teleport"] = AsciiSprite({
        " .----.",
        "|  @  |",
        "| / \\ |",
        " `----'"
    }, Color::BrightMagenta, Color::Default, false);

    items_["item.bomb"] = AsciiSprite({
        "  .--.",
        " (    )",
        "  `--'  *"
    }, Color::BrightRed, Color::Default, false);

    items_["item.key"] = AsciiSprite({
        " .--.",
        " (  )--",
        " `--'"
    }, Color::Steel, Color::Default, false);

    items_["item.guide_fragment_common"] = AsciiSprite({
        " .----.",
        "| || | |",
        "| || | |",
        " `----'"
    }, Color::White, Color::Default, false);

    items_["item.guide_fragment_rare"] = AsciiSprite({
        " .====.",
        "| || | |",
        "| || | |",
        " `===='"
    }, Color::Gold, Color::Default, true);

    items_["item.lore_scroll"] = AsciiSprite({
        " .----.",
        "| :: | |",
        "| :: | |",
        " `----'"
    }, Color::BrightYellow, Color::Default, false);

    items_["item.gold"] = AsciiSprite({
        " .--.",
        "( $$ )",
        " `--'"
    }, Color::Gold, Color::Default, true);

    items_["item.ruby"] = AsciiSprite({
        "  /\\",
        " /  \\",
        " \\  /",
        "  \\/"
    }, Color::BrightRed, Color::Default, true);
}

void AsciiArtLibrary::initTiles() {
    tiles_[0] = AsciiSprite({"#"}, Color::Steel, Color::Default, false);
    tiles_[1] = AsciiSprite({"."}, Color::Gray, Color::Default, false);
    tiles_[5] = AsciiSprite({">"}, Color::BrightYellow, Color::Default, true);
    tiles_[6] = AsciiSprite({"<"}, Color::BrightYellow, Color::Default, true);
    tiles_[16] = AsciiSprite({"A"}, Color::BrightMagenta, Color::Default, true);
    tiles_[17] = AsciiSprite({"F"}, Color::BrightCyan, Color::Default, true);
    tiles_[18] = AsciiSprite({"C"}, Color::Gold, Color::Default, true);
}

void AsciiArtLibrary::initBanners() {
    banners_["stone"] = AsciiSprite({
        "  ____  _                  ____             _   _     ",
        " / ___|| |_ ___  _ __   __|  _ \\  ___ _ __ | |_| |__  ",
        " \\___ \\| __/ _ \\| '_ \\ / _` | | |/ _ \\ '_ \\| __| '_ \\ ",
        "  ___) | || (_) | | | | (_| |_| |  __/ |_) | |_| | | |",
        " |____/ \\__\\___/|_| |_|\\__,____/ \\___| .__/ \\__|_| |_|",
        "                                     |_|              "
    }, Color::Steel, Color::Default, true);

    banners_["fungal"] = AsciiSprite({
        "  _____                     _   ____  _                         ",
        " |  ___|   _ _ __   __ _  _| | | __ )| | ___   ___  _ __ ___     ",
        " | |_ | | | | '_ \\ / _` |/ _` | |  _ \\| |/ _ \\ / _ \\| '_ ` _ \\    ",
        " |  _|| |_| | | | | (_| | (_| | | |_) | | (_) | (_) | | | | | |   ",
        " |_|   \\__,_|_| |_|\\__, |\\__,_| |____/|_|\\___/ \\___/|_| |_| |_|   ",
        "                   |___/                                         "
    }, Color::Green, Color::Default, true);

    banners_["title"] = AsciiSprite({
        "  ____  _   _    _    ____   _____        __  ____   _____ _____ ____  ",
        " / ___|| | | |  / \\  |  _ \\ / _ \\ \\      / / |  _ \\ | ____| ____|  _ \\ ",
        " \\___ \\| |_| | / _ \\ | | | | | | \\ \\ /\\ / /  | | | ||  _| |  _| | |_) |",
        "  ___) |  _  |/ ___ \\| |_| | |_| |\\ V  V /   | |_| || |___| |___|  __/ ",
        " |____/|_| |_/_/   \\_\\____/ \\___/  \\_/\\_/    |____/ |_____|_____|_|    ",
        "                                                                       ",
        "  ___        __  _       _ _   _       ____             _   _           ",
        " |_ _|_ __  / _|(_)_ __ (_) |_| |__   |  _ \\  ___ _ __ | |_| |__  ___   ",
        "  | || '_ \\| |_ | | '_ \\| | __| '_ \\  | | | |/ _ \\ '_ \\| __| '_ \\/ __|  ",
        "  | || | | |  _|| | | | | | |_| | | | | |_| |  __/ |_) | |_| | | \\__ \\  ",
        " |___|_| |_|_|  |_|_| |_|_|\\__|_| |_| |____/ \\___| .__/ \\__|_| |_|___/  ",
        "                                                 |_|                  "
    }, Color::BrightYellow, Color::Default, true);
}

AsciiSprite AsciiArtLibrary::getMonsterSprite(const std::string& stableId) const {
    auto it = monsters_.find(stableId);
    if (it != monsters_.end()) return it->second;
    return AsciiSprite({"?"}, Color::White, Color::Default, false);
}

AsciiSprite AsciiArtLibrary::getItemSprite(const std::string& stableId) const {
    auto it = items_.find(stableId);
    if (it != items_.end()) return it->second;
    return AsciiSprite({"?"}, Color::White, Color::Default, false);
}

AsciiSprite AsciiArtLibrary::getTileSprite(int tileId) const {
    auto it = tiles_.find(tileId);
    if (it != tiles_.end()) return it->second;
    return AsciiSprite({"."}, Color::Gray, Color::Default, false);
}

AsciiSprite AsciiArtLibrary::getBiomeBanner(const std::string& biome) const {
    auto it = banners_.find(biome);
    if (it != banners_.end()) return it->second;
    auto it2 = banners_.find("stone");
    if (it2 != banners_.end()) return it2->second;
    return AsciiSprite({"SHADOWDEEP"}, Color::White, Color::Default, true);
}

AsciiSprite AsciiArtLibrary::getTitleArt() const {
    auto it = banners_.find("title");
    if (it != banners_.end()) return it->second;
    return AsciiSprite({"SHADOWDEEP"}, Color::BrightYellow, Color::Default, true);
}

std::vector<std::string> AsciiArtLibrary::getMonsterNames() const {
    std::vector<std::string> names;
    for (auto& p : monsters_) names.push_back(p.first);
    return names;
}

std::vector<std::string> AsciiArtLibrary::getItemNames() const {
    std::vector<std::string> names;
    for (auto& p : items_) names.push_back(p.first);
    return names;
}

namespace ascii_draw {

void drawSprite(ScreenBuffer& screen, int x, int y, const AsciiSprite& sprite) {
    for (int i = 0; i < sprite.height; ++i) {
        if (i >= static_cast<int>(sprite.lines.size())) break;
        screen.text(x, y + i, sprite.lines[i], sprite.fg, sprite.bold);
    }
}

void drawSpriteClipped(ScreenBuffer& screen, int x, int y, const AsciiSprite& sprite, int maxW, int maxH) {
    for (int i = 0; i < sprite.height && i < maxH; ++i) {
        if (i >= static_cast<int>(sprite.lines.size())) break;
        std::string line = sprite.lines[i];
        if (static_cast<int>(line.size()) > maxW) line = line.substr(0, maxW);
        screen.text(x, y + i, line, sprite.fg, sprite.bold);
    }
}

void drawBoxStyled(ScreenBuffer& screen, int x, int y, int w, int h, Color border, BoxStyle style, bool bold) {
    if (w < 2 || h < 2) return;
    std::string tl, tr, bl, br, horiz, vert;
    switch (style) {
        case BoxStyle::Single:
            tl = "+"; tr = "+"; bl = "+"; br = "+"; horiz = "-"; vert = "|";
            break;
        case BoxStyle::Double:
            tl = "#"; tr = "#"; bl = "#"; br = "#"; horiz = "="; vert = "#";
            break;
        case BoxStyle::Thick:
            tl = "#"; tr = "#"; bl = "#"; br = "#"; horiz = "#"; vert = "#";
            break;
        case BoxStyle::Ascii:
            tl = "+"; tr = "+"; bl = "+"; br = "+"; horiz = "-"; vert = "|";
            break;
        case BoxStyle::Rounded:
            tl = "."; tr = "."; bl = "'"; br = "'"; horiz = "-"; vert = "|";
            break;
    }
    screen.set(x, y, tl, border, bold);
    screen.set(x + w - 1, y, tr, border, bold);
    screen.set(x, y + h - 1, bl, border, bold);
    screen.set(x + w - 1, y + h - 1, br, border, bold);
    for (int i = 1; i < w - 1; ++i) {
        screen.set(x + i, y, horiz, border, bold);
        screen.set(x + i, y + h - 1, horiz, border, bold);
    }
    for (int i = 1; i < h - 1; ++i) {
        screen.set(x, y + i, vert, border, bold);
        screen.set(x + w - 1, y + i, vert, border, bold);
    }
}

void drawProgressBar(ScreenBuffer& screen, int x, int y, int w, int current, int max, Color fill, Color empty, Color border) {
    if (w < 3) return;
    drawBoxStyled(screen, x, y, w, 3, border, BoxStyle::Single, false);
    if (max <= 0) max = 1;
    float pct = static_cast<float>(current) / static_cast<float>(max);
    if (pct < 0) pct = 0;
    if (pct > 1) pct = 1;
    int innerW = w - 2;
    int filled = static_cast<int>(static_cast<float>(innerW) * pct);
    for (int i = 0; i < innerW; ++i) {
        if (i < filled) screen.set(x + 1 + i, y + 1, "#", fill, true);
        else screen.set(x + 1 + i, y + 1, "-", empty, false);
    }
    std::string txt = std::to_string(current) + "/" + std::to_string(max);
    int tx = x + (w - static_cast<int>(txt.size())) / 2;
    screen.text(tx, y + 1, txt, Color::BrightWhite, true);
}

void drawVerticalBar(ScreenBuffer& screen, int x, int y, int h, int current, int max, Color fill, Color empty) {
    if (h < 2) return;
    if (max <= 0) max = 1;
    float pct = static_cast<float>(current) / static_cast<float>(max);
    if (pct < 0) pct = 0;
    if (pct > 1) pct = 1;
    int filled = static_cast<int>(static_cast<float>(h) * pct);
    for (int i = 0; i < h; ++i) {
        if (i < h - filled) screen.set(x, y + i, ".", empty, false);
        else screen.set(x, y + i, "#", fill, true);
    }
}

void drawCenteredText(ScreenBuffer& screen, int y, const std::string& text, Color fg, bool bold) {
    int w = screen.width();
    int x = (w - static_cast<int>(text.size())) / 2;
    if (x < 0) x = 0;
    screen.text(x, y, text, fg, bold);
}

void drawWrappedText(ScreenBuffer& screen, int x, int y, int w, int h, const std::string& text, Color fg, bool bold) {
    int curY = y;
    int curX = x;
    std::string word;
    for (size_t i = 0; i <= text.size(); ++i) {
        char c = (i < text.size()) ? text[i] : ' ';
        if (c == ' ' || c == '\n' || i == text.size()) {
            if (!word.empty()) {
                if (curX + static_cast<int>(word.size()) > x + w) {
                    curX = x;
                    curY++;
                    if (curY >= y + h) break;
                }
                screen.text(curX, curY, word, fg, bold);
                curX += static_cast<int>(word.size()) + 1;
                word.clear();
            }
            if (c == '\n') {
                curX = x;
                curY++;
                if (curY >= y + h) break;
            }
        } else {
            word += c;
        }
    }
}

void drawLine(ScreenBuffer& screen, int x0, int y0, int x1, int y1, const std::string& ch, Color c) {
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int x = x0;
    int y = y0;
    while (true) {
        screen.set(x, y, ch, c, false);
        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
}

void drawGradientText(ScreenBuffer& screen, int x, int y, const std::string& text, Color from, Color to, bool bold) {
    for (size_t i = 0; i < text.size(); ++i) {
        float t = text.size() > 1 ? static_cast<float>(i) / static_cast<float>(text.size() - 1) : 0.0f;
        Color c = lerpColor(from, to, t);
        std::string ch(1, text[i]);
        screen.set(x + static_cast<int>(i), y, ch, c, bold);
    }
}

void drawBorderedText(ScreenBuffer& screen, int x, int y, const std::string& text, Color fg, Color border, bool bold) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            screen.text(x + dx, y + dy, text, border, false);
        }
    }
    screen.text(x, y, text, fg, bold);
}

void clearArea(ScreenBuffer& screen, int x, int y, int w, int h) {
    for (int yy = y; yy < y + h; ++yy) {
        for (int xx = x; xx < x + w; ++xx) {
            screen.set(xx, yy, " ", Color::Default, false);
        }
    }
}

void fillArea(ScreenBuffer& screen, int x, int y, int w, int h, const std::string& ch, Color fg, Color bg, bool bold) {
    for (int yy = y; yy < y + h; ++yy) {
        for (int xx = x; xx < x + w; ++xx) {
            Cell cell;
            cell.grapheme = ch;
            cell.fg = fg;
            cell.bg = bg;
            cell.bold = bold;
            screen.set(xx, yy, cell);
        }
    }
}

void drawShadowBox(ScreenBuffer& screen, int x, int y, int w, int h, Color border, Color shadow) {
    drawBoxStyled(screen, x, y, w, h, border, BoxStyle::Single, true);
    for (int i = 1; i < w; ++i) {
        screen.set(x + i, y + h, "#", shadow, false);
    }
    for (int i = 1; i < h; ++i) {
        screen.set(x + w, y + i, "#", shadow, false);
    }
}

void drawDoubleBuffer(ScreenBuffer& screen, int x, int y, int w, int h) {
    drawBoxStyled(screen, x, y, w, h, Color::White, BoxStyle::Double, true);
    drawBoxStyled(screen, x + 1, y + 1, w - 2, h - 2, Color::Gray, BoxStyle::Single, false);
}

Color lerpColor(Color a, Color b, float t) {
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    if (t < 0.5f) return a;
    return b;
}

Color healthColor(int hp, int maxHp) {
    if (maxHp <= 0) return Color::BrightRed;
    float pct = static_cast<float>(hp) / static_cast<float>(maxHp);
    if (pct > 0.66f) return Color::BrightGreen;
    if (pct > 0.33f) return Color::Yellow;
    return Color::BrightRed;
}

Color rarityColor(int rarity) {
    switch (rarity) {
        case 0: return Color::White;
        case 1: return Color::BrightGreen;
        case 2: return Color::BrightCyan;
        case 3: return Color::Purple;
        case 4: return Color::Gold;
        case 5: return Color::BrightYellow;
        default: return Color::White;
    }
}

}

}
