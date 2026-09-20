# Changelog

## Zv1 - Zero v1 - Early Alpha (In Development)

### Added

- Multi-file C++20 architecture replacing single-file prototype
- CMake build system with presets, Makefile frontend
- Cross-platform terminal backend abstraction (POSIX termios + Windows Console API)
- Platform paths: Windows %APPDATA%, Linux XDG, macOS Application Support
- Binary save format with magic, versioning, chunk-based payload, atomic saving, backups
- SaveManager with Entries: New, Open, Rename, Delete (trash), Duplicate, Backup, Export, Import
- Config system with JSON, schema versioning, atomic save
- TUI ScreenBuffer with virtual screen, diff rendering, Unicode-aware
- Home screen: Entries, Settings, Changelog, Credits, Quit
- GameSession: 30 depths, 10 regions (Forgotten Cellars, Goblin Warrens, Sunken Crypts, Fungal Depths, Crystal Caverns, Drowned Halls, Infernal Foundry, Ashen Fortress, Abyssal Temple, Shadowdeep)
- Dungeon generation: room-and-corridor, BSP, cavern cellular automata, features (water, lava, fountains, chests, rubble), connectivity validation
- FOV: Bresenham + shadowcast ready, explored memory
- Monsters: 30+ templates with families, AI states (Idle, Wandering, Suspicious, Alert, Hunting, Fleeing, Guarding), pathfinding BFS, door opening, erratic, flying
- Combat: damage types (Physical, Fire, Frost, Lightning, Poison, Holy, Shadow, Arcane), critical hits, armor, resistances, status effects
- Status effects: Poison, Bleeding, Burning, Chilled, Frozen, Stunned, Confused, Blinded, Rooted, Haste, Slow, Regeneration, Shielded, Cursed, Blessed, Fear, Strength
- Player classes: Warrior, Rogue, Ranger, Arcanist, Cleric, Warden with distinct starting stats
- Items: weapons, armor, shields, helmets, boots, rings, amulets, potions, food, scrolls, bombs, keys, gold, rubies, Amulet of Shadowdeep
- Rarity: Common, Uncommon, Rare, Epic, Legendary, Mythic
- Enchantments: Flaming, Frostbound, Stormforged, Venomous, Vampiric, Swift, Fortified, Lucky, Radiant, Shadow-touched, Thorned, Echoing
- Inventory: 26 slots, stacking, equipment slots, comparison
- Economy: Gold common, Rubies rare, no microtransactions
- CLI: --version (Zv1), --about, --license, --help, --seed, --check-update, --update, --entry
- GitHub Actions CI for Tier1 platforms, release artifacts with OS/arch naming
- Documentation: README, BUILDING, PORTING, THIRD_PARTY, CHANGELOG, CONTRIBUTING
- Tests for RNG determinism, dungeon connectivity, save roundtrip, paths, combat

### Changed

- Project no longer single file, now modular
- Terminal handling abstracted, no longer POSIX-only
- Rendering uses virtual buffer instead of direct ANSI
- Game logic separated from platform

### Fixed

- Windows compilation: termios.h not found - solved architecturally via backend abstraction
- Wander shuffle creating new mt19937 each call - now uses main RNG engine
- Double blit in inventory screen
- Gold auto-pickup only first pile
- Boss placement on stairs edge case
- Global depth mutation hack in ensureFloor replaced with parameter passing
- Terminal left raw on crash - RAII wrapper

### Known Issues

- Balancing ongoing
- Some regions share generation parameters, visual differentiation needs more content
- Shops, NPCs, quests, dialogue, blacksmith, enchanter partially implemented as data-driven stubs
- Update manager currently checks GitHub Releases but does not auto-install in this build
- FTXUI integration optional, default uses custom renderer
