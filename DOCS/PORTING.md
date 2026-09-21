# Porting SHADOWDEEP

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game.

It is designed to work for desktop users and CLI-based distros and OSes, including minimal environments, BSDs, and research systems where a TUI is possible.

We are always open to ports:

- You can create a PR for your own version or port
- If the PR's CI fails, we will fix bugs ourselves if you request it
- You can also open an ISSUE with label `port-request` requesting a custom port

See `.github/ISSUE_TEMPLATE/port_request.md` and `.github/pull_request_template.md` for templates.

## Architecture

SHADOWDEEP uses explicit platform abstraction. No `#ifdef _WIN32` in gameplay code.

Platform-specific code lives only in:

- `src/platform/`
- `include/shadowdeep/platform/`

Gameplay code never includes `windows.h` or `termios.h` directly.

## Interfaces to Implement

### ITerminalBackend

File: `include/shadowdeep/platform/terminal_backend.hpp`

Methods:

- `enterRaw()`: Enter raw mode, disable echo, line buffering
- `restore()`: Restore original mode
- `isRaw()`: Is in raw mode
- `getSize()`: Terminal size cols/rows
- `pollKey()`: Non-blocking key poll
- `waitKey()`: Blocking wait
- `writeRaw(string)`: Write ANSI
- `hideCursor()`, `showCursor()`, `clearScreen()`

POSIX implementation: `src/platform/terminal_backend.cpp` (termios, ioctl, read)

Windows implementation: same file (Win32 Console API, ENABLE_VIRTUAL_TERMINAL_PROCESSING, ReadConsoleInputW)

To port, implement these methods for your OS. If your OS has no termios and no Win32 console, provide a minimal shim that satisfies the same contract, even if it only supports blocking reads.

### PlatformPaths

File: `include/shadowdeep/platform/paths.hpp`

- `getAppPaths()`: Return dataRoot, configRoot, cacheRoot, savesDir, etc.
- `ensureAppDirs()`: Create directories
- `getExecutableDir()`, `getExecutablePath()`

Implementations must respect:

- Windows: `%APPDATA%\shadowdeep\` via SHGetKnownFolderPath(FOLDERID_RoamingAppData), NOT `%USERPROFILE%\AppData\Roaming`
- Linux: XDG Base Directory spec with fallbacks: `$XDG_DATA_HOME/shadowdeep`, `$XDG_CONFIG_HOME/shadowdeep`, `$XDG_STATE_HOME/shadowdeep`, `$XDG_CACHE_HOME/shadowdeep`, falling back to `~/.local/share`, `~/.config`, etc.
- macOS: `~/Library/Application Support/shadowdeep/` for data, `~/Library/Preferences/shadowdeep/` or Application Support for config, `~/Library/Caches/shadowdeep/`
- BSD: XDG-like or `~/.local/share/shadowdeep`

Ensure atomic save compatibility: temp file + flush + fsync + atomic rename.

## Adding a New OS

1. Create `src/platform/<os>/terminal_<os>.cpp`
2. Create `src/platform/<os>/paths_<os>.cpp`
3. Implement interfaces
4. Add to CMakeLists.txt with condition: check `CMAKE_SYSTEM_NAME` and `CMAKE_SYSTEM_PROCESSOR`
5. Provide fallback TUI renderer if FTXUI unavailable: implement minimal ANSI renderer that satisfies IRenderer
6. Update `DOCS/BUILDING.md` with build instructions
7. Update this file with Tier placement

Example CMake addition:

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  list(APPEND PLATFORM_SOURCES src/platform/openbsd/terminal_openbsd.cpp src/platform/openbsd/paths_openbsd.cpp)
endif()
```

## TUI Abstraction

- `include/shadowdeep/tui/screen.hpp`: virtual screen buffer, diff rendering, resize handling, Unicode width
- Primary renderer: custom ANSI (works everywhere)
- Optional: FTXUI (MIT) via FetchContent when `SHADOWDEEP_USE_FTXUI=ON`, provides enhanced widgets, still satisfies same interface for Tier3 fallback
- Fallback chain: True color -> 256 -> 16 -> 8 -> ASCII. Never require true color.

If porting to exotic system (e.g., Haiku, SerenityOS, TempleOS research), you can implement a minimal backend that only handles 80x24 and basic input, and we will help improve it.

## Save System

Binary format is endian-fixed (little-endian), no raw struct dumps. Must work across architectures.

See DOCS/SAVE_FORMAT.md. Save header uses magic, versioning, CRCs. Validation is mandatory.

## Testing a Port

- Build: `cmake --preset debug && cmake --build --preset debug`
- Run: `./build/debug/shadowdeep --version` must output Zv1
- Run: `./build/debug/shadowdeep --help`, `--about`, `--license`
- Non-TTY: `echo | ./build/debug/shadowdeep` should error gracefully but info commands must still work
- Run tests: `ctest --preset debug`
- Manual: Home screen, Entries, new game, move, inventory, save, load, export/import

## Tier Definitions

- Tier 1: Officially built and tested CI — Windows x86_64/ARM64, Linux x86_64/AArch64, macOS ARM64
- Tier 2: Supported, built where infra permits — Linux ARMv7, RISC-V64, FreeBSD x86_64
- Tier 3: Experimental/community — OpenBSD, NetBSD, DragonFly, Haiku, Alpine musl
- Tier 4: Research/future — SerenityOS, WASM TUI, TempleOS-inspired, other custom kernels

If you port to Tier 3/4, we will add CI if possible, or mark as community-maintained.

## Contribution Flow for Ports

1. Fork, create branch `port/<os>-<arch>`
2. Implement backend
3. Open PR using template `.github/pull_request_template.md`
4. If CI fails, comment `@maintainers I need help fixing this port` — we will look and fix bugs ourselves if you request it
5. Alternatively, open Issue using `port-request` template and we will triage

We promise not to close valid port PRs without trying to help.

## Dependencies for Porting

- C++20 compiler required
- CMake 3.20+
- Ninja or Make
- Optional: FTXUI, fmt, nlohmann_json, cpr/libcurl, zstd — all are cross-platform and MIT/BSD compatible

If your target OS lacks a dependency, we can conditionally disable it (e.g., no update checker on offline-only ports).

See DOCS/THIRD_PARTY.md for evaluated dependencies.
