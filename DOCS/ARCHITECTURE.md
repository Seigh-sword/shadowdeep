# SHADOWDEEP Architecture

## Overview

SHADOWDEEP Zv1 is a modular C++20 project, transformed from a single-file prototype into a maintainable multi-file architecture capable of growing for years.

## Repository Structure

```
shadowdeep/
  CMakeLists.txt
  CMakePresets.json
  Makefile (frontend for CMake)
  LICENSE (ISC)
  README.md
  DOCS/
    BUILDING.md
    PORTING.md
    THIRD_PARTY.md
    CHANGELOG.md
    CONTRIBUTING.md
    ARCHITECTURE.md
    SAVE_FORMAT.md
  include/shadowdeep/
    version.hpp
    core/
    platform/
    terminal/
    tui/
    world/
    entities/
    combat/
    items/
    effects/
    quests/
    npc/
    shops/
    codex/
    game/
    save/
    config/
    app/
    update/
  src/
    platform/
    terminal/
    tui/
    world/
    entities/
    combat/
    items/
    effects/
    quests/
    npc/
    shops/
    codex/
    game/
    save/
    config/
    app/
    update/
    main.cpp
  data/
    monsters/
    items/
    regions/
    quests/
  tests/
  .github/
    workflows/
    ISSUE_TEMPLATE/
    pull_request_template.md
```

## Module Dependencies

- `platform` - no deps, defines ITerminalBackend, PlatformPaths
- `core` - Vec2, Rect, Rng
- `terminal` - Color, Input, depends on platform
- `tui` - ScreenBuffer virtual screen, diff rendering, depends on terminal
- `world` - Tile, Dungeon, Generator, Fov, Region, depends on core
- `entities` - Player, Monster, depends on core, world, effects, items
- `items` - Item, Inventory, Equipment, Enchantment
- `effects` - StatusEffect, EffectManager
- `combat` - DamageType, Damage
- `quests` - Quest, QuestManager
- `npc` - Npc templates
- `shops` - Shop, ShopItem
- `codex` - Bestiary
- `game` - GameSession, Floor, GameMessage, depends on all above
- `save` - BinaryReader/Writer, SaveHeader, EntryMetadata, SaveManager, Migration
- `config` - Config, ConfigManager
- `app` - Cli, App (home, entries, gameplay, settings, changelog, credits)
- `update` - UpdateManager (HTTPS, verification)

Dependency direction is strictly downward, no cycles. Platform code never leaks into gameplay.

## Platform Abstraction

All platform-specific code lives in `src/platform/` and implements interfaces in `include/shadowdeep/platform/`:

- `ITerminalBackend`: enterRaw, restore, getSize, pollKey, waitKey, writeRaw, hideCursor, showCursor, clearScreen
  - POSIX: termios, ioctl, read, nanosleep
  - Windows: Win32 Console API, ENABLE_VIRTUAL_TERMINAL_PROCESSING, ReadConsoleInputW
- `PlatformPaths`: getAppPaths, ensureAppDirs, getExecutableDir
  - Windows: SHGetKnownFolderPath(FOLDERID_RoamingAppData) -> %APPDATA%\shadowdeep\
  - Linux: XDG Base Directory
  - macOS: ~/Library/Application Support/

Gameplay code never includes windows.h or termios.h.

## TUI

- `ScreenBuffer`: virtual screen of Cells (grapheme, fg, bg, bold, etc.), toAnsi() with diff rendering
- Primary backend: custom ANSI renderer (works everywhere)
- Optional: FTXUI (MIT) via FetchContent when `SHADOWDEEP_USE_FTXUI=ON`, provides enhanced widgets, still satisfies same interface for Tier3 fallback

## Save System

See SAVE_FORMAT.md for details. Binary container with magic, versioning, chunk-based payload, atomic saving, backups, migration.

## Content

Data-driven where possible:

- `data/regions/regions.json` - thematic regions
- `data/monsters/monsters.json` - monster templates (stable IDs)
- `data/items/` - item templates
- Hardcoded templates in `src/entities/monster.cpp`, `src/items/item.cpp` as fallback

Stable IDs like `monster.ancient_dragon`, `item.amulet_shadowdeep` are used for save compatibility, not display names.

## Build

CMake primary, Makefile frontend. Presets for debug, release, relwithdebinfo, plus Windows Visual Studio presets.

## CI

GitHub Actions:

- ci.yml: Tier1 builds (Windows x86_64, Linux x86_64 GCC/Clang, macOS ARM64), sanitizers, clang-tidy
- cross.yml: Tier2 cross-compilation via dockcross (aarch64, armv7, riscv64) and FreeBSD VM
- release.yml: Release artifacts named `shadowdeep-Zv1-<os>-<arch>.tar.gz/zip`

## Versioning

Game version constant `kGameVersion = "Zv1"` in version.hpp, stays Zv1 until owner says. Save schema, config schema, build revision independent.
