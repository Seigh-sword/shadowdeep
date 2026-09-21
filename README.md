# SHADOWDEEP

<p align="center">
<nobr>
<img src="https://github.com/Seigh-sword/shadowdeep/actions/workflows/ci.yml/badge.svg?branch=main" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://github.com/Seigh-sword/shadowdeep/actions/workflows/cross.yml/badge.svg?branch=main" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://github.com/Seigh-sword/shadowdeep/actions/workflows/release.yml/badge.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/license-ISC-blue.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/version-Zv1-orange.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/CMake-3.20%2B-green.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS%20%7C%20BSD-lightgrey.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/Raspberry%20Pi-aarch64%20%7C%20armv7-C51A4A.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/arch-x86_64%20%7C%20ARM64%20%7C%20ARMv7%20%7C%20RISC--V64-yellow.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/TUI-ANSI%20%2B%20Unicode%20%2B%20FTXUI-informational.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/save-binary%20%2B%20atomic%20%2B%20backups-success.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/economy-no%20MTX%20%7C%20Gold%20%26%20Rubies-ff69b4.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/offline-capable-brightgreen.svg" height="20">
<span>&nbsp;✦&nbsp;</span>
<img src="https://img.shields.io/badge/portability-intentional%20%2B%20Tier1--4-blueviolet.svg" height="20">
</nobr>
</p>

**Zv1 - Zero v1 - Early Alpha**

SHADOWDEEP is a roguelike dungeon crawler written in C++.

A surprisingly deep and polished game that happens to run inside a terminal.

Made by: github.com/Seigh-sword

Repository: https://github.com/Seigh-sword/shadowdeep

Licensed under the ISC License.

## Intentionally Built for Porting

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game.

It is designed to work for:

- Desktop users on Windows, Linux, macOS
- CLI-based distros and minimal environments
- BSDs and other Unix-like systems
- Raspberry Pi (aarch64 and armv7) and other ARM boards
- Unusual and research operating systems where a TUI is possible

Portability is architectural, not an afterthought. Platform-specific code lives only in `src/platform/` behind clear interfaces. Gameplay never includes `windows.h` or `termios.h` directly.

If you want SHADOWDEEP on your OS, you are welcome to try.

- You can create a PR for your own version or port
- If the PR's CI fails, we will help fix the bugs ourselves if you request it
- You can also open an ISSUE requesting a custom port and we will look into it

See `DOCS/PORTING.md` for what a new backend needs to implement.

We are always open to port contributions.

## Badges Explained

- CI: Tier1 builds Windows x86_64 MSVC, Linux x86_64 GCC/Clang, macOS ARM64, sanitizers, clang-tidy
- Cross Compile: Tier2 builds Linux aarch64, armv7, riscv64 via dockcross, FreeBSD via VM, Raspberry Pi via same
- Release: Builds release artifacts named `shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz` with checksums
- License ISC, Version Zv1 constant until owner changes, C++20, CMake 3.20+
- Platform and Arch coverage, TUI with ANSI + Unicode + optional FTXUI, binary save with atomic + backups, no MTX, offline capable, intentional portability

## Features

- Turn-based tactical dungeon crawling
- Terminal TUI with responsive layout, Unicode and ASCII fallback
- 30 depths across 10 thematic regions
- Procedural generation: rooms, BSP, caverns, vaults
- 30+ monsters with distinct AI (pack hunting, fleeing, summoning, ranged)
- Boss system with phases, including the Ancient Dragon
- Combat with damage types, critical hits, armor, resistances
- Status effects with duration and stacking
- 6 classes: Warrior, Rogue, Ranger, Arcanist, Cleric, Warden
- Items: weapons, armor, potions, scrolls, food, bombs, keys
- Rarity: Common, Uncommon, Rare, Epic, Legendary, Mythic
- Enchantments: Flaming, Frostbound, Stormforged, Vampiric, etc.
- Equipment slots and comparison
- Gold and Rubies economy (no microtransactions, all earned in-game)
- Shops, NPCs, rescues, quests, dialogue
- Field of view with explored memory
- Look/examine mode, targeting, map screen
- Bestiary and Codex
- Entries: persistent save system with binary format, atomic saving, backups, import/export
- Settings: color, theme, animations, keybindings
- Seeded runs, --seed support
- Cross-platform: Windows, Linux, macOS, BSDs, Raspberry Pi, with Tier system
- Update checker/installer: --check-update/--update with OS/arch detection, HTTPS, checksum verification, staged updates

## Quick Start

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep
```

Or using Makefile:

```bash
make
./build/debug/shadowdeep
```

## Controls

- Arrow keys / hjkl / yubn: Move or attack
- g / , : Pick up
- i : Inventory
- q : Quaff potion
- r : Read scroll
- > : Descend stairs
- < : Ascend stairs
- z / . : Wait
- ? : Help
- Q : Quit

Home screen:

- Entries: Manage saves
- Settings: Configure
- Changelog: See changes
- Credits: Attribution
- Quit: Exit

## CLI

- `shadowdeep` : Launch game
- `shadowdeep --version` : Show Zv1
- `shadowdeep --about` : About info
- `shadowdeep --license` : License
- `shadowdeep --help` : Help
- `shadowdeep --seed <N>` : Deterministic seed
- `shadowdeep --check-update` : Check for updates (detects OS and arch, queries GitHub releases)
- `shadowdeep --update` : Download correct artifact for your OS/arch, verify checksum, stage update

Non-TTY: info commands work, gameplay shows error if no TTY.

## User Data Locations

- Windows: `%APPDATA%\shadowdeep\`
- Linux: XDG compliant: `$XDG_DATA_HOME/shadowdeep` (fallback `~/.local/share/shadowdeep`)
- macOS: `~/Library/Application Support/shadowdeep/`
- Raspberry Pi: same as Linux XDG

Structure:

- saves/
- save_backups/
- config/
- logs/
- cache/
- updates/
- exports/
- run_history/
- crash_reports/

## Data Folder Location

`data/` stays at repository root, not inside `include/` or `src/`, because:

- It is content (monsters, items, regions, quests), not source code
- CMake installs it to `share/shadowdeep/data`
- Keeps content versioned separately and easy to package into release artifacts `shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz`
- For ports, data is read-only and located via `getExecutableDir() + /share/shadowdeep/data` or via `AppPaths`
- If it were inside `src/`, it would mix content with code and break XDG separation

See `AGENTS.md` for full explanation and `DOCS/ARCHITECTURE.md` for module dependencies.

Stable IDs like `monster.ancient_dragon`, `item.amulet_shadowdeep` are used for save compatibility.

## Building

See DOCS/BUILDING.md

Quick for Raspberry Pi:

```bash
# On Pi directly
sudo apt install build-essential cmake ninja-build
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version

# Cross compile for Pi from x86_64
docker pull dockcross/linux-arm64:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake -B build/rpi-aarch64 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake --build build/rpi-aarch64

# For older Pi (armv7)
docker pull dockcross/linux-armv7:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake -B build/rpi-armv7 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake --build build/rpi-armv7
```

## Porting

See DOCS/PORTING.md - intentionally designed for broad OS support including Raspberry Pi.

We welcome custom ports. Open an Issue with label `port-request` or a PR with your port. If CI fails, we will help fix it if you ask.

Tiers:

- Tier1: Windows x86_64/ARM64, Linux x86_64/AArch64, macOS ARM64
- Tier2: Linux ARMv7, RISC-V64, FreeBSD x86_64, Raspberry Pi aarch64/armv7
- Tier3: OpenBSD, NetBSD, DragonFly, Haiku, Alpine musl
- Tier4: Research/future - SerenityOS, WASM TUI, etc.

## Raspberry Pi Port

Raspberry Pi is officially supported as Tier2:

- Pi 3/4/5 64-bit: `linux-aarch64` artifact, also named `linux-arm64-rpi`
- Pi Zero/1/2 32-bit: `linux-armv7` artifact, also named `linux-armv7-rpi`
- OS: Raspberry Pi OS (Debian-based) or any Linux
- Build: same as Linux, XDG paths
- Terminal: works with Pi default terminal, fallback ASCII
- CI: built via dockcross in cross.yml, tested via QEMU
- Release: artifacts `shadowdeep-Zv1-linux-aarch64.tar.gz` and `shadowdeep-Zv1-linux-armv7.tar.gz` work on Pi, plus explicit `shadowdeep-Zv1-raspberrypi-aarch64.tar.gz` alias in release notes

See DOCS/PORTING.md for details.

## Update System

Without releases, `--check-update` and `--update` will not work, so release workflow must publish artifacts.

Update manager does:

- Detects OS: windows, linux, macos, freebsd, etc. via preprocessor and runtime
- Detects arch: x86_64, arm64, aarch64, armv7, riscv64 via preprocessor
- Constructs artifact name: `shadowdeep-Zv1-{os}-{arch}.zip` (Windows) or `.tar.gz` (Unix)
- Queries GitHub API `https://api.github.com/repos/Seigh-sword/shadowdeep/releases/latest` via HTTPS only (libcurl or WinHTTP)
- Parses JSON for version, size, sha256, url
- Compares with current `kGameVersion` and `kBuildRevision`
- Downloads correct file to `cache/updates/`, verifies size/checksum/signature
- Never overwrites saves, stages update, handles Windows self-replace via staged updater
- CLI: `--check-update` prints available version and URL, `--update` performs download

Release workflow `.github/workflows/release.yml`:

- Builds Tier1 and Raspberry Pi artifacts
- Generates SHA256 checksums
- Creates GitHub release via `softprops/action-gh-release` with artifacts
- Runs on tags `Zv1*` and `v*` and manual dispatch, plus can be triggered for nightly

## Documentation

All docs are in DOCS/:

- DOCS/BUILDING.md
- DOCS/PORTING.md
- DOCS/THIRD_PARTY.md
- DOCS/CHANGELOG.md
- DOCS/CONTRIBUTING.md
- DOCS/ARCHITECTURE.md
- DOCS/SAVE_FORMAT.md
- DOCS/README.md - index

Root docs:

- README.md - this file
- LICENSE - ISC
- AGENTS.md - instructions for AI agents and contributors (see AGENT.md link)
- CMakeLists.txt, CMakePresets.json, Makefile

## License

ISC License - see LICENSE

## Third Party

See DOCS/THIRD_PARTY.md

## Contributing

See DOCS/CONTRIBUTING.md - we are always open to PRs, including ports to new platforms. See also AGENTS.md.

We are always open:

- Create a PR for your own version or port
- If CI fails, we will fix bugs ourselves if you request it
- Or open an Issue with label `port-request`

## AGENTS.md

See AGENTS.md for AI agent instructions. Also accessible as AGENT.md (symlink concept). The file explains versioning, no comments in code, platform abstraction, data folder location, CI monitoring, release and update system, Raspberry Pi port, and more.
