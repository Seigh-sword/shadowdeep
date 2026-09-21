#include "shadowdeep/terminal/color.hpp"

namespace shadowdeep {

std::string ansiColorCode(Color c, bool bg) {
    int base = bg ? 40 : 30;
    switch (c) {
        case Color::Default: return bg ? "\033[49m" : "\033[39m";
        case Color::Black: return std::string("\033[") + std::to_string(base) + "m";
        case Color::Gray: return "\033[90m";
        case Color::White: return "\033[37m";
        case Color::Red: return "\033[31m";
        case Color::Green: return "\033[32m";
        case Color::Yellow: return "\033[33m";
        case Color::Blue: return "\033[34m";
        case Color::Magenta: return "\033[35m";
        case Color::Cyan: return "\033[36m";
        case Color::BrightBlack: return "\033[90m";
        case Color::BrightWhite: return "\033[97m";
        case Color::BrightRed: return "\033[91m";
        case Color::BrightGreen: return "\033[92m";
        case Color::BrightYellow: return "\033[93m";
        case Color::BrightCyan: return "\033[96m";
        case Color::BrightMagenta: return "\033[95m";
        case Color::Brown: return "\033[38;5;130m";
        case Color::Gold: return "\033[38;5;220m";
        case Color::Steel: return "\033[38;5;246m";
        case Color::Orange: return "\033[38;5;208m";
        case Color::Purple: return "\033[38;5;129m";
        default: return "\033[39m";
    }
}

std::string ansiReset() {
    return "\033[0m";
}

std::string ansiBold(bool on) {
    return on ? "\033[1m" : "\033[22m";
}

Rgb colorToRgb(Color c) {
    switch (c) {
        case Color::Black: return {0,0,0};
        case Color::Gray: return {128,128,128};
        case Color::White: return {200,200,200};
        case Color::Red: return {200,0,0};
        case Color::Green: return {0,180,0};
        case Color::Yellow: return {200,200,0};
        case Color::Blue: return {0,0,200};
        case Color::Magenta: return {200,0,200};
        case Color::Cyan: return {0,200,200};
        case Color::BrightWhite: return {255,255,255};
        case Color::BrightRed: return {255,80,80};
        case Color::BrightGreen: return {80,255,80};
        case Color::BrightYellow: return {255,255,100};
        case Color::BrightCyan: return {80,255,255};
        case Color::BrightMagenta: return {255,80,255};
        case Color::Brown: return {139,69,19};
        case Color::Gold: return {255,215,0};
        case Color::Steel: return {150,150,150};
        case Color::Orange: return {255,165,0};
        case Color::Purple: return {128,0,128};
        default: return {200,200,200};
    }
}

Color rgbToNearest256(const Rgb& rgb) {
    (void)rgb;
    return Color::White;
}

}
