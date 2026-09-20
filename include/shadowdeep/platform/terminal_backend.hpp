#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <memory>

namespace shadowdeep {

enum class KeyCode : int {
    None = -1,
    Escape = 1005,
    Up = 1001,
    Down = 1002,
    Left = 1003,
    Right = 1004,
    Enter = 13,
    Backspace = 127,
    Tab = 9,
    F1 = 1101,
    F2 = 1102,
    F3 = 1103,
    F4 = 1104,
    F5 = 1105,
    F6 = 1106,
    F7 = 1107,
    F8 = 1108,
    F9 = 1109,
    F10 = 1110,
    F11 = 1111,
    F12 = 1112
};

struct TerminalSize {
    int cols = 80;
    int rows = 24;
};

struct KeyEvent {
    int code = static_cast<int>(KeyCode::None);
    char ch = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;

    bool isChar() const {
        return code >= 32 && code <= 126;
    }

    bool isArrow() const {
        return code == static_cast<int>(KeyCode::Up) ||
               code == static_cast<int>(KeyCode::Down) ||
               code == static_cast<int>(KeyCode::Left) ||
               code == static_cast<int>(KeyCode::Right);
    }
};

class ITerminalBackend {
public:
    virtual ~ITerminalBackend() = default;
    virtual bool enterRaw() = 0;
    virtual void restore() = 0;
    virtual bool isRaw() const = 0;
    virtual TerminalSize getSize() = 0;
    virtual KeyEvent pollKey() = 0;
    virtual KeyEvent waitKey() = 0;
    virtual void writeRaw(const std::string& s) = 0;
    virtual void hideCursor() = 0;
    virtual void showCursor() = 0;
    virtual void clearScreen() = 0;
};

std::unique_ptr<ITerminalBackend> createTerminalBackend();

}
