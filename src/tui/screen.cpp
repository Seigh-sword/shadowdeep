#include "shadowdeep/tui/screen.hpp"
#include "shadowdeep/terminal/color.hpp"
#include <algorithm>

namespace shadowdeep {

ScreenBuffer::ScreenBuffer(int w, int h) : w_(w), h_(h) {
    cells_.resize(w * h);
    prev_.resize(w * h);
    clear();
}

void ScreenBuffer::resize(int w, int h) {
    w_ = w;
    h_ = h;
    cells_.assign(w * h, Cell{});
    prev_.assign(w * h, Cell{});
    clear();
}

int ScreenBuffer::width() const { return w_; }
int ScreenBuffer::height() const { return h_; }

void ScreenBuffer::clear() {
    for (auto& c : cells_) {
        c.grapheme = " ";
        c.fg = Color::Default;
        c.bg = Color::Default;
        c.bold = false;
        c.underline = false;
        c.dim = false;
    }
}

void ScreenBuffer::set(int x, int y, const Cell& cell) {
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return;
    cells_[y * w_ + x] = cell;
}

void ScreenBuffer::set(int x, int y, const std::string& g, Color fg, bool bold) {
    Cell c;
    c.grapheme = g;
    c.fg = fg;
    c.bold = bold;
    set(x, y, c);
}

void ScreenBuffer::put(int x, int y, char ch, Color fg, bool bold) {
    std::string s(1, ch);
    set(x, y, s, fg, bold);
}

void ScreenBuffer::text(int x, int y, const std::string& s, Color fg, bool bold) {
    for (size_t i = 0; i < s.size(); ++i) {
        if (x + static_cast<int>(i) >= w_) break;
        put(x + static_cast<int>(i), y, s[i], fg, bold);
    }
}

void ScreenBuffer::textClipped(int x, int y, int maxW, const std::string& s, Color fg, bool bold) {
    std::string clipped = s.substr(0, maxW);
    text(x, y, clipped, fg, bold);
}

void ScreenBuffer::fillRect(int x, int y, int w, int h, const Cell& cell) {
    for (int yy = y; yy < y + h; ++yy) {
        for (int xx = x; xx < x + w; ++xx) {
            set(xx, yy, cell);
        }
    }
}

void ScreenBuffer::drawBox(int x, int y, int w, int h, Color border) {
    if (w < 2 || h < 2) return;
    put(x, y, '+', border, true);
    put(x + w - 1, y, '+', border, true);
    put(x, y + h - 1, '+', border, true);
    put(x + w - 1, y + h - 1, '+', border, true);
    for (int i = 1; i < w - 1; ++i) {
        put(x + i, y, '-', border, true);
        put(x + i, y + h - 1, '-', border, true);
    }
    for (int i = 1; i < h - 1; ++i) {
        put(x, y + i, '|', border, true);
        put(x + w - 1, y + i, '|', border, true);
    }
}

void ScreenBuffer::drawHLine(int x, int y, int w, const std::string& ch, Color c) {
    for (int i = 0; i < w; ++i) set(x + i, y, ch, c, false);
}

void ScreenBuffer::drawVLine(int x, int y, int h, const std::string& ch, Color c) {
    for (int i = 0; i < h; ++i) set(x, y + i, ch, c, false);
}

const Cell& ScreenBuffer::at(int x, int y) const {
    static Cell empty;
    if (x < 0 || x >= w_ || y < 0 || y >= h_) return empty;
    return cells_[y * w_ + x];
}

Cell& ScreenBuffer::at(int x, int y) {
    return cells_[y * w_ + x];
}

std::string ScreenBuffer::toAnsi(bool diff) {
    std::string out;
    out.reserve(w_ * h_ * 2);
    out += "\033[H";

    Color lastFg = Color::Default;
    Color lastBg = Color::Default;
    bool lastBold = false;

    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) {
            const Cell& c = cells_[y * w_ + x];
            if (diff) {
                const Cell& pc = prev_[y * w_ + x];
                if (pc.grapheme == c.grapheme && pc.fg == c.fg && pc.bg == c.bg && pc.bold == c.bold) {
                    continue;
                }
            }

            if (c.bold != lastBold) {
                out += "\033[0m";
                lastFg = Color::Default;
                lastBg = Color::Default;
                lastBold = c.bold;
                if (c.bold) out += "\033[1m";
            }

            if (c.fg != lastFg) {
                out += ansiColorCode(c.fg, false);
                lastFg = c.fg;
            }

            if (c.bg != lastBg && c.bg != Color::Default) {
                out += ansiColorCode(c.bg, true);
                lastBg = c.bg;
            }

            out += c.grapheme;
        }
        if (y + 1 < h_) out += "\r\n";
        out += "\033[0m";
        lastFg = Color::Default;
        lastBg = Color::Default;
        lastBold = false;
    }

    prev_ = cells_;
    return out;
}

}
