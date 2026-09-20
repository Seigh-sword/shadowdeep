#pragma once
#include "../platform/terminal_backend.hpp"

namespace shadowdeep {

class Input {
public:
    explicit Input(ITerminalBackend& backend) : backend_(backend) {}

    KeyEvent poll() {
        return backend_.pollKey();
    }

    KeyEvent wait() {
        return backend_.waitKey();
    }

private:
    ITerminalBackend& backend_;
};

}
