# SHADOWDEEP Documentation

All project documentation lives here.

## Index

- BUILDING.md - How to build on Tier1 platforms, cross-compilation
- PORTING.md - Platform abstraction, how to add a new OS/arch backend, porting tiers
- THIRD_PARTY.md - Third-party licenses
- CHANGELOG.md - Zv1 changelog
- CONTRIBUTING.md - Contributing rules, version rule (Zv1 stays Zv1), no comments in code, no emojis
- ARCHITECTURE.md - Repository architecture, modules, dependencies
- SAVE_FORMAT.md - Binary save format, atomic saving, migration

## Quick Links

- Project: https://github.com/Seigh-sword/shadowdeep
- Author: github.com/Seigh-sword
- Version: Zv1 (Zero v1) - early alpha, custom versioning, not SemVer
- License: ISC

## Porting Intent

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game.

It targets:

- Desktop users (Windows, Linux, macOS)
- CLI-based distros and minimal environments
- BSDs and other Unix-like systems
- Research/unusual OSes where TUI is possible

We are always open:

- Create a PR for your own version or port
- If CI fails, we will fix bugs ourselves if you request it
- Or open an Issue with label `port-request` for requesting a custom port

See PORTING.md for details.
