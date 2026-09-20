#include "shadowdeep/platform/terminal_backend.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#endif

namespace shadowdeep {

#ifdef _WIN32

class WindowsTerminalBackend : public ITerminalBackend {
public:
    WindowsTerminalBackend() {
        hIn_ = GetStdHandle(STD_INPUT_HANDLE);
        hOut_ = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    ~WindowsTerminalBackend() override {
        restore();
    }

    bool enterRaw() override {
        if (raw_) return true;

        if (!GetConsoleMode(hIn_, &origInMode_)) return false;
        if (!GetConsoleMode(hOut_, &origOutMode_)) return false;

        DWORD inMode = origInMode_;
        inMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
        inMode |= ENABLE_WINDOW_INPUT;

        DWORD outMode = origOutMode_;
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;

        if (!SetConsoleMode(hIn_, inMode)) return false;
        if (!SetConsoleMode(hOut_, outMode)) {
            SetConsoleMode(hIn_, origInMode_);
            return false;
        }

        raw_ = true;
        hideCursor();
        clearScreen();
        return true;
    }

    void restore() override {
        if (!raw_) return;
        SetConsoleMode(hIn_, origInMode_);
        SetConsoleMode(hOut_, origOutMode_);
        raw_ = false;
        showCursor();
        writeRaw("\033[0m");
    }

    bool isRaw() const override {
        return raw_;
    }

    TerminalSize getSize() override {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(hOut_, &info)) {
            return {80, 24};
        }
        int cols = info.srWindow.Right - info.srWindow.Left + 1;
        int rows = info.srWindow.Bottom - info.srWindow.Top + 1;
        if (cols < 10) cols = 80;
        if (rows < 10) rows = 24;
        return {cols, rows};
    }

    KeyEvent pollKey() override {
        DWORD count = 0;
        if (!GetNumberOfConsoleInputEvents(hIn_, &count) || count == 0) {
            return {static_cast<int>(KeyCode::None), 0, false, false, false};
        }

        INPUT_RECORD rec{};
        DWORD read = 0;
        if (!PeekConsoleInputW(hIn_, &rec, 1, &read) || read == 0) {
            return {static_cast<int>(KeyCode::None), 0, false, false, false};
        }

        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
            ReadConsoleInputW(hIn_, &rec, 1, &read);
            return translateKey(rec.Event.KeyEvent);
        } else {
            ReadConsoleInputW(hIn_, &rec, 1, &read);
            return {static_cast<int>(KeyCode::None), 0, false, false, false};
        }
    }

    KeyEvent waitKey() override {
        for (;;) {
            INPUT_RECORD rec{};
            DWORD read = 0;
            if (!ReadConsoleInputW(hIn_, &rec, 1, &read)) {
                Sleep(5);
                continue;
            }
            if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
                return translateKey(rec.Event.KeyEvent);
            }
        }
    }

    void writeRaw(const std::string& s) override {
        DWORD written = 0;
        WriteConsoleA(hOut_, s.c_str(), static_cast<DWORD>(s.size()), &written, nullptr);
    }

    void hideCursor() override {
        writeRaw("\033[?25l");
    }

    void showCursor() override {
        writeRaw("\033[?25h");
    }

    void clearScreen() override {
        writeRaw("\033[2J\033[H");
    }

private:
    KeyEvent translateKey(const KEY_EVENT_RECORD& ke) {
        KeyEvent ev;
        ev.ctrl = (ke.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
        ev.alt = (ke.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
        ev.shift = (ke.dwControlKeyState & SHIFT_PRESSED) != 0;

        switch (ke.wVirtualKeyCode) {
            case VK_UP: ev.code = static_cast<int>(KeyCode::Up); break;
            case VK_DOWN: ev.code = static_cast<int>(KeyCode::Down); break;
            case VK_LEFT: ev.code = static_cast<int>(KeyCode::Left); break;
            case VK_RIGHT: ev.code = static_cast<int>(KeyCode::Right); break;
            case VK_ESCAPE: ev.code = static_cast<int>(KeyCode::Escape); break;
            case VK_RETURN: ev.code = static_cast<int>(KeyCode::Enter); break;
            case VK_BACK: ev.code = static_cast<int>(KeyCode::Backspace); break;
            case VK_TAB: ev.code = static_cast<int>(KeyCode::Tab); break;
            case VK_F1: ev.code = static_cast<int>(KeyCode::F1); break;
            case VK_F2: ev.code = static_cast<int>(KeyCode::F2); break;
            case VK_F3: ev.code = static_cast<int>(KeyCode::F3); break;
            case VK_F4: ev.code = static_cast<int>(KeyCode::F4); break;
            case VK_F5: ev.code = static_cast<int>(KeyCode::F5); break;
            case VK_F6: ev.code = static_cast<int>(KeyCode::F6); break;
            case VK_F7: ev.code = static_cast<int>(KeyCode::F7); break;
            case VK_F8: ev.code = static_cast<int>(KeyCode::F8); break;
            case VK_F9: ev.code = static_cast<int>(KeyCode::F9); break;
            case VK_F10: ev.code = static_cast<int>(KeyCode::F10); break;
            case VK_F11: ev.code = static_cast<int>(KeyCode::F11); break;
            case VK_F12: ev.code = static_cast<int>(KeyCode::F12); break;
            default: {
                if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 126) {
                    ev.code = ke.uChar.AsciiChar;
                    ev.ch = ke.uChar.AsciiChar;
                } else if (ke.wVirtualKeyCode >= 0x41 && ke.wVirtualKeyCode <= 0x5A) {
                    char c = static_cast<char>(ke.wVirtualKeyCode);
                    if (!ev.shift) c = static_cast<char>(c + 32);
                    ev.code = c;
                    ev.ch = c;
                } else {
                    ev.code = static_cast<int>(KeyCode::None);
                }
                break;
            }
        }
        if (ev.ch == 0 && ev.code >= 32 && ev.code <= 126) {
            ev.ch = static_cast<char>(ev.code);
        }
        return ev;
    }

    HANDLE hIn_{};
    HANDLE hOut_{};
    DWORD origInMode_{};
    DWORD origOutMode_{};
    bool raw_ = false;
};

#else

class PosixTerminalBackend : public ITerminalBackend {
public:
    PosixTerminalBackend() = default;
    ~PosixTerminalBackend() override {
        restore();
    }

    bool enterRaw() override {
        if (raw_) return true;
        if (!isatty(STDIN_FILENO)) return false;
        if (tcgetattr(STDIN_FILENO, &orig_) != 0) return false;

        termios t = orig_;
        t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        t.c_oflag &= ~(OPOST);
        t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        t.c_cflag |= CS8;
        t.c_cc[VMIN] = 0;
        t.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t) != 0) return false;
        raw_ = true;
        hideCursor();
        clearScreen();
        return true;
    }

    void restore() override {
        if (!raw_) return;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_);
        raw_ = false;
        showCursor();
        writeRaw("\033[0m");
    }

    bool isRaw() const override {
        return raw_;
    }

    TerminalSize getSize() override {
        winsize ws{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
            return {80, 24};
        }
        int cols = ws.ws_col;
        int rows = ws.ws_row;
        if (cols < 10) cols = 80;
        if (rows < 10) rows = 24;
        return {cols, rows};
    }

    KeyEvent pollKey() override {
        unsigned char c = 0;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n != 1) {
            return {static_cast<int>(KeyCode::None), 0, false, false, false};
        }

        if (c != 27) {
            if (c == 127) {
                return {static_cast<int>(KeyCode::Backspace), 0, false, false, false};
            }
            if (c == '\r' || c == '\n') {
                return {static_cast<int>(KeyCode::Enter), '\n', false, false, false};
            }
            if (c < 32) {
                KeyEvent ev;
                ev.code = c;
                ev.ctrl = true;
                return ev;
            }
            return {static_cast<int>(c), static_cast<char>(c), false, false, false};
        }

        timespec ts{0, 15000000};
        nanosleep(&ts, nullptr);

        unsigned char buf[8]{};
        ssize_t m = read(STDIN_FILENO, buf, 8);
        if (m <= 0) {
            return {static_cast<int>(KeyCode::Escape), 0, false, false, false};
        }

        if (m >= 2 && buf[0] == '[') {
            switch (buf[1]) {
                case 'A': return {static_cast<int>(KeyCode::Up), 0, false, false, false};
                case 'B': return {static_cast<int>(KeyCode::Down), 0, false, false, false};
                case 'C': return {static_cast<int>(KeyCode::Right), 0, false, false, false};
                case 'D': return {static_cast<int>(KeyCode::Left), 0, false, false, false};
                default: break;
            }
            if (m >= 3 && buf[1] >= '1' && buf[1] <= '9') {
                if (buf[m-1] == '~') {
                    int num = buf[1] - '0';
                    if (num == 1) return {static_cast<int>(KeyCode::Escape), 0, false, false, false};
                    if (num == 3) return {127, 0, false, false, false};
                }
            }
        } else if (m >= 1 && buf[0] == 'O') {
            if (m >= 2) {
                switch (buf[1]) {
                    case 'P': return {static_cast<int>(KeyCode::F1), 0, false, false, false};
                    case 'Q': return {static_cast<int>(KeyCode::F2), 0, false, false, false};
                    case 'R': return {static_cast<int>(KeyCode::F3), 0, false, false, false};
                    case 'S': return {static_cast<int>(KeyCode::F4), 0, false, false, false};
                    default: break;
                }
            }
        }

        return {static_cast<int>(KeyCode::Escape), 0, false, false, false};
    }

    KeyEvent waitKey() override {
        for (;;) {
            KeyEvent ev = pollKey();
            if (ev.code != static_cast<int>(KeyCode::None)) return ev;
            timespec ts{0, 5000000};
            nanosleep(&ts, nullptr);
        }
    }

    void writeRaw(const std::string& s) override {
        fwrite(s.data(), 1, s.size(), stdout);
        fflush(stdout);
    }

    void hideCursor() override {
        writeRaw("\033[?25l");
    }

    void showCursor() override {
        writeRaw("\033[?25h");
    }

    void clearScreen() override {
        writeRaw("\033[2J\033[H");
    }

private:
    termios orig_{};
    bool raw_ = false;
};

#endif

std::unique_ptr<ITerminalBackend> createTerminalBackend() {
#ifdef _WIN32
    return std::make_unique<WindowsTerminalBackend>();
#else
    return std::make_unique<PosixTerminalBackend>();
#endif
}

}
