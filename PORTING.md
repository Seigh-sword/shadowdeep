# Porting SHADOWDEEP

## Architecture

SHADOWDEEP uses explicit platform abstraction. No `#ifdef _WIN32` in gameplay code.

Platform-specific code lives only in:

- `src/platform/`
- `include/shadowdeep/platform/`

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

### PlatformPaths

File: `include/shadowdeep/platform/paths.hpp`

- `getAppPaths()`: Return dataRoot, configRoot, cacheRoot, savesDir, etc.
- `ensureAppDirs()`: Create directories
- `getExecutableDir()`, `getExecutablePath()`

Implementations must respect:

- Windows: `%APPDATA%\shadowdeep\` via SHGetKnownFolderPath(FOLDERID_RoamingAppData)
- Linux: XDG Base Directory spec
- macOS: ~/Library/Application Support/shadowdeep/
- BSD: XDG-like

## Adding a New OS

1. Create `src/platform/<os>/terminal_<os>.cpp`
2. Create `src/platform/<os>/paths_<os>.cpp`
3. Implement interfaces
4. Add to CMakeLists.txt with condition
5. Provide fallback TUI renderer if FTXUI unavailable: implement `IRenderer` minimal ANSI

## TUI Abstraction

- `tui/screen.hpp`: virtual screen buffer, diff rendering
- If porting to exotic system (e.g., TempleOS), implement custom backend that satisfies only required methods

## Save System

Binary format is endian-fixed (little-endian), no raw struct dumps. Must work across architectures.

## Testing a Port

- Build: `cmake --preset debug && cmake --build --preset debug`
- Run: `./build/debug/shadowdeep --version` must output Zv1
- Run: `./build/debug/shadowdeep --help`
- Run tests: `ctest --preset debug`
- Manual: Home screen, Entries, new game, move, inventory, save

## Tier Definitions

- Tier 1: Officially built and tested CI
- Tier 2: Supported, built where infra permits
- Tier 3: Experimental/community
- Tier 4: Research/future

See README for current tiers.
