# SHADOWDEEP

**Zv1 — Zero v1 — Early Alpha**

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
- Unusual and research operating systems where a TUI is possible

Portability is architectural, not an afterthought. Platform-specific code lives only in `src/platform/` behind clear interfaces. Gameplay never includes `windows.h` or `termios.h` directly.

If you want SHADOWDEEP on your OS, you are welcome to try.

- You can create a PR for your own version or port
- If the PR's CI fails, we will help fix the bugs ourselves if you request it
- You can also open an ISSUE requesting a custom port and we will look into it

See `DOCS/PORTING.md` for what a new backend needs to implement.

We are always open to port contributions.

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
- Cross-platform: Windows, Linux, macOS, BSDs, with Tier system

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

See DOCS/BUILDING.md

## Porting

See DOCS/PORTING.md — intentionally designed for broad OS support.

We welcome custom ports. Open an Issue with label `port-request` or a PR with your port. If CI fails, we will help fix it if you ask.

## Documentation

All docs are in DOCS/:

- DOCS/BUILDING.md
- DOCS/PORTING.md
- DOCS/THIRD_PARTY.md
- DOCS/CHANGELOG.md
- DOCS/CONTRIBUTING.md
- DOCS/ARCHITECTURE.md (coming)
- DOCS/SAVE_FORMAT.md (coming)

## License

ISC License — see LICENSE

## Third Party

See DOCS/THIRD_PARTY.md

## Contributing

See DOCS/CONTRIBUTING.md — we are always open to PRs, including ports to new platforms.
