#pragma once
#include <vector>
#include <string>
#include "../terminal/color.hpp"
#include "../core/vec2.hpp"

namespace shadowdeep {

struct Cell {
    std::string grapheme = " ";
    Color fg = Color::Default;
    Color bg = Color::Default;
    bool bold = false;
    bool underline = false;
    bool dim = false;
};

class ScreenBuffer {
public:
    ScreenBuffer(int w = 80, int h = 24);

    void resize(int w, int h);
    int width() const;
    int height() const;

    void clear();
    void set(int x, int y, const Cell& cell);
    void set(int x, int y, const std::string& g, Color fg = Color::Default, bool bold = false);
    void put(int x, int y, char ch, Color fg = Color::Default, bool bold = false);
    void text(int x, int y, const std::string& s, Color fg = Color::Default, bool bold = false);
    void textClipped(int x, int y, int maxW, const std::string& s, Color fg = Color::Default, bool bold = false);
    void fillRect(int x, int y, int w, int h, const Cell& cell);
    void drawBox(int x, int y, int w, int h, Color border = Color::White);
    void drawHLine(int x, int y, int w, const std::string& ch = "-", Color c = Color::White);
    void drawVLine(int x, int y, int h, const std::string& ch = "|", Color c = Color::White);

    const Cell& at(int x, int y) const;
    Cell& at(int x, int y);

    std::string toAnsi(bool diff = false);

private:
    int w_ = 80;
    int h_ = 24;
    std::vector<Cell> cells_;
    std::vector<Cell> prev_;
};

}
