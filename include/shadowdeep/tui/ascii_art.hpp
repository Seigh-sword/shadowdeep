#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "../terminal/color.hpp"
#include "screen.hpp"

namespace shadowdeep {

struct AsciiSprite {
    std::vector<std::string> lines;
    Color fg = Color::White;
    Color bg = Color::Default;
    bool bold = false;
    int width = 0;
    int height = 0;

    AsciiSprite() = default;
    AsciiSprite(std::vector<std::string> l, Color f = Color::White, Color b = Color::Default, bool bo = false)
        : lines(std::move(l)), fg(f), bg(b), bold(bo) {
        height = static_cast<int>(lines.size());
        width = 0;
        for (auto& ln : lines) if (static_cast<int>(ln.size()) > width) width = static_cast<int>(ln.size());
    }
};

enum class BoxStyle {
    Single,
    Double,
    Thick,
    Ascii,
    Rounded
};

class AsciiArtLibrary {
public:
    static AsciiArtLibrary& instance();

    AsciiSprite getMonsterSprite(const std::string& stableId) const;
    AsciiSprite getItemSprite(const std::string& stableId) const;
    AsciiSprite getTileSprite(int tileId) const;
    AsciiSprite getBiomeBanner(const std::string& biome) const;
    AsciiSprite getTitleArt() const;

    std::vector<std::string> getMonsterNames() const;
    std::vector<std::string> getItemNames() const;

private:
    AsciiArtLibrary();
    void initMonsters();
    void initItems();
    void initTiles();
    void initBanners();

    std::unordered_map<std::string, AsciiSprite> monsters_;
    std::unordered_map<std::string, AsciiSprite> items_;
    std::unordered_map<int, AsciiSprite> tiles_;
    std::unordered_map<std::string, AsciiSprite> banners_;
};

namespace ascii_draw {

void drawSprite(ScreenBuffer& screen, int x, int y, const AsciiSprite& sprite);
void drawSpriteClipped(ScreenBuffer& screen, int x, int y, const AsciiSprite& sprite, int maxW, int maxH);
void drawBoxStyled(ScreenBuffer& screen, int x, int y, int w, int h, Color border, BoxStyle style, bool bold = true);
void drawProgressBar(ScreenBuffer& screen, int x, int y, int w, int current, int max, Color fill, Color empty, Color border);
void drawVerticalBar(ScreenBuffer& screen, int x, int y, int h, int current, int max, Color fill, Color empty);
void drawCenteredText(ScreenBuffer& screen, int y, const std::string& text, Color fg, bool bold = false);
void drawWrappedText(ScreenBuffer& screen, int x, int y, int w, int h, const std::string& text, Color fg, bool bold = false);
void drawLine(ScreenBuffer& screen, int x0, int y0, int x1, int y1, const std::string& ch, Color c);
void drawGradientText(ScreenBuffer& screen, int x, int y, const std::string& text, Color from, Color to, bool bold = false);
void drawBorderedText(ScreenBuffer& screen, int x, int y, const std::string& text, Color fg, Color border, bool bold = false);
void clearArea(ScreenBuffer& screen, int x, int y, int w, int h);
void fillArea(ScreenBuffer& screen, int x, int y, int w, int h, const std::string& ch, Color fg, Color bg, bool bold = false);
void drawShadowBox(ScreenBuffer& screen, int x, int y, int w, int h, Color border, Color shadow);
void drawDoubleBuffer(ScreenBuffer& screen, int x, int y, int w, int h);

Color lerpColor(Color a, Color b, float t);
Color healthColor(int hp, int maxHp);
Color rarityColor(int rarity);

}

}
