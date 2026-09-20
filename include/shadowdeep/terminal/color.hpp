#pragma once
#include <cstdint>
#include <string>

namespace shadowdeep {

enum class Color : uint16_t {
    Default = 0,
    Black = 1,
    Gray = 2,
    White = 3,
    Red = 4,
    Green = 5,
    Yellow = 6,
    Blue = 7,
    Magenta = 8,
    Cyan = 9,
    BrightBlack = 10,
    BrightWhite = 11,
    BrightRed = 12,
    BrightGreen = 13,
    BrightYellow = 14,
    BrightCyan = 15,
    BrightMagenta = 16,
    Brown = 17,
    Gold = 18,
    Steel = 19,
    Orange = 20,
    Purple = 21
};

struct Rgb {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

std::string ansiColorCode(Color c, bool bg = false);
std::string ansiReset();
std::string ansiBold(bool on);

Rgb colorToRgb(Color c);
Color rgbToNearest256(const Rgb& rgb);

}
