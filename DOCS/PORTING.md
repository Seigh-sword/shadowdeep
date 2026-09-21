# Porting SHADOWDEEP

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game.

It is designed to work for desktop users and CLI-based distros and OSes, including minimal environments, BSDs, Raspberry Pi, and research systems where a TUI is possible.

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
- Raspberry Pi: same as Linux XDG, `~/.local/share/shadowdeep/`

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

## Raspberry Pi Port

Raspberry Pi is Tier2 officially supported.

- OS: Linux (Raspberry Pi OS Debian-based or any Linux)
- Arch: aarch64 for Pi 3/4/5 64-bit, armv7 for Pi Zero/1/2 32-bit
- Build: same as Linux, no special code needed, just C++20 compiler and CMake
- Presets: `rpi-aarch64` and `rpi-armv7` in CMakePresets.json
- CI: built via dockcross in `cross.yml` raspberrypi job and `release.yml` build-cross job, using `dockcross/linux-arm64:latest` and `dockcross/linux-armv7:latest`, tested via QEMU
- Paths: XDG like Linux
- Terminal: works with Pi default terminal, LXTerminal, fallback ASCII
- Artifacts: `shadowdeep-Zv1-raspberrypi-aarch64.tar.gz`, `shadowdeep-Zv1-raspberrypi-armv7.tar.gz`, also aliased as `shadowdeep-Zv1-linux-aarch64.tar.gz`, `shadowdeep-Zv1-linux-arm64.tar.gz`, `shadowdeep-Zv1-linux-armv7.tar.gz`, `shadowdeep-Zv1-linux-arm64-rpi.tar.gz`, `shadowdeep-Zv1-linux-armv7-rpi.tar.gz`
- Update: `shadowdeep --check-update` detects `linux-aarch64` or `linux-armv7` and downloads correct artifact, verifies SHA256, stages to `~/.cache/shadowdeep/updates/`
- Install: `tar xzf shadowdeep-Zv1-raspberrypi-aarch64.tar.gz && ./shadowdeep/shadowdeep --version`

No separate source tree needed, same `src/platform/` POSIX backend works. If you have Pi-specific input quirks, you can add `src/platform/rpi/` but currently not needed.

## Save System

Binary format is endian-fixed (little-endian), no raw struct dumps. Must work across architectures including ARM.

See DOCS/SAVE_FORMAT.md. Save header uses magic, versioning, CRCs. Validation is mandatory.

## Testing a Port

- Build: `cmake --preset debug && cmake --build --preset debug`
- Run: `./build/debug/shadowdeep --version` must output Zv1
- Run: `./build/debug/shadowdeep --help`, `--about`, `--license`
- Non-TTY: `echo | ./build/debug/shadowdeep` should error gracefully but info commands must still work
- Run tests: `ctest --preset debug`
- Manual: Home screen, Entries, new game, move, inventory, save, load, export/import
- For Pi: test on actual Pi hardware if possible, or via QEMU, check terminal size, input, save/load

## Tier Definitions

- Tier 1: Officially built and tested CI - Windows x86_64/ARM64, Linux x86_64/AArch64, macOS ARM64
- Tier 2: Supported, built where infra permits - Linux ARMv7, RISC-V64, FreeBSD x86_64, Raspberry Pi aarch64/armv7 (Pi 3/4/5 and Zero/1/2)
- Tier 3: Experimental/community - OpenBSD, NetBSD, DragonFly, Haiku, Alpine musl
- Tier 4: Research/future - SerenityOS, WASM TUI, TempleOS-inspired, other custom kernels

If you port to Tier 3/4, we will add CI if possible, or mark as community-maintained.

## Contribution Flow for Ports

1. Fork, create branch `port/<os>-<arch>` e.g., `port/rpi-aarch64`
2. Implement backend if needed (for Pi, no backend needed, just test)
3. Open PR using template `.github/pull_request_template.md`
4. If CI fails, comment `@maintainers I need help fixing this port` - we will look and fix bugs ourselves if you request it
5. Alternatively, open Issue using `port-request` template and we will triage

We promise not to close valid port PRs without trying to help.

## Dependencies for Porting

- C++20 compiler required
- CMake 3.20+
- Ninja or Make
- Optional: FTXUI, fmt, nlohmann_json, cpr/libcurl, zstd - all are cross-platform and MIT/BSD compatible
- For Pi: `libcurl4-openssl-dev` for update checker, else fallback to curl binary

If your target OS lacks a dependency, we can conditionally disable it (e.g., no update checker on offline-only ports) via `SHADOWDEEP_USE_CURL=OFF`.

See DOCS/THIRD_PARTY.md for evaluated dependencies.

## Update System and OS/Arch Detection

Update manager must detect OS and arch to download correct executable and dynamic libraries:

- OS detection via preprocessor: _WIN32 -> windows, __APPLE__ -> macos, __FreeBSD__ -> freebsd, __linux__ -> linux (including Pi)
- Arch detection: __x86_64__/_M_X64 -> x86_64, __aarch64__/_M_ARM64 -> aarch64, __arm__ -> armv7, __riscv -> riscv64
- Artifact naming: `shadowdeep-Zv1-{os}-{arch}.tar.gz` or `.zip` for Windows
- For Pi: `raspberrypi-aarch64` and `raspberrypi-armv7` are aliases, update manager tries candidates list
- Download via HTTPS only: WinHTTP on Windows, libcurl on POSIX, fallback to curl binary
- Verify size and SHA256, never overwrite saves, stage to cache/updates/
- Handle Windows self-replace via staged updater

See `include/shadowdeep/update/update_manager.hpp` and `src/update/update_manager.cpp` for implementation.
