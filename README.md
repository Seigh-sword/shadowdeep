# SHADOWDEEP

**Zv1 — Zero v1 — Early Alpha**

SHADOWDEEP is a roguelike dungeon crawler written in C++.

A surprisingly deep and polished game that happens to run inside a terminal.

Made by: github.com/Seigh-sword

Repository: https://github.com/Seigh-sword/shadowdeep

Licensed under the ISC License.

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
- Cross-platform: Windows, Linux, macOS, BSDs

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
- `shadowdeep --check-update` : Check for updates
- `shadowdeep --update` : Update

## User Data Locations

- Windows: `%APPDATA%\shadowdeep\`
- Linux: XDG compliant: `$XDG_DATA_HOME/shadowdeep` (fallback `~/.local/share/shadowdeep`)
- macOS: `~/Library/Application Support/shadowdeep/`

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

## Building

See BUILDING.md

## Porting

See PORTING.md

## License

ISC License — see LICENSE

## Third Party

See THIRD_PARTY.md
