# AGENTS.md - Instructions for AI Agents and Contributors

This file is for AI agents (and humans) working on SHADOWDEEP.

## Project Identity

- Name: SHADOWDEEP
- Version: Zv1 - Zero v1 - Early Alpha - custom versioning, not SemVer. Do NOT increment Zv1 to Zv2 or 1.0.0 without owner explicit approval. Save schema, config schema, build revision may change independently.
- Author: github.com/Seigh-sword
- Repository: https://github.com/Seigh-sword/shadowdeep
- License: ISC - must be kept, see LICENSE and DOCS/THIRD_PARTY.md
- Source of truth: https://github.com/Seigh-sword/shadowdeep - Gist no longer relevant

## Core Rules

- No comments in code: no // or /* */ in .cpp/.hpp files. Documentation in markdown only.
- No emojis in code or public docs unless explicitly requested. Keep professional.
- No M-dashes or En-dashes in docs - use hyphen - instead. User requested removal.
- C++20 or newer, modern RAII, no raw owning pointers where RAII works, clear names, small cohesive components.
- No age in public files, do not talk down.
- Project name SHADOWDEEP, author github.com/Seigh-sword, ISC License kept.
- Home screen menu uses term "Entries" for active saves. Internal code uses SaveFile/SaveManager etc. Chronicles separate for dead/victory history. Do not rename Entries without asking.
- Keep installation and user data separate: Windows %APPDATA%\shadowdeep\, Linux XDG_DATA/CONFIG/STATE/CACHE with fallbacks, macOS Application Support, BSD conventions.
- Binary save must not raw-dump structs: use fixed-width ints, explicit endian, string/array lengths, validation, size limits, treat as untrusted.
- Save header: magic, container version, save schema version, UUID, timestamps, payload size, compression, checksum; chunk-based optional skip.
- Atomic save via temp file + flush + sync + atomic replace; preserve backup before migration; if newer schema seen, refuse with message "This Entry was saved by a newer..."
- Updating: support --check-update --update, HTTPS only, mature library, no silent install, verify size/checksum/signature, never overwrite saves, handle Windows self-replace via staged updater.
- Platform abstraction mandatory: no scattered #ifdef _WIN32 in gameplay; no direct windows.h/termios.h/unistd.h in game code; implement PlatformPaths/TerminalBackend/InputBackend.
- Portability tiers explicit; Tier1 initial Windows x86_64/ARM64, Linux x86_64/AArch64, macOS ARM64; do not claim fake support.
- Terminal: research Windows Console API/Windows Terminal vs POSIX; UTF-8 grapheme width, differential rendering, resize, keyboard normalization, fallback ASCII/8/16/256/true-color.
- CLI must support --version (prints Zv1), --about, --license, --help, --check-update, --update, --seed <seed>; non-TTY gameplay shows error but info commands work.
- Gold and Rubies both earned in-game, no premium currency, no microtransactions, no telemetry by default, offline capable.
- Research dependencies before choosing: evaluate license/platform/arch/maintenance/security/CMake/package-manager; candidates FTXUI/fmt/spdlog/CLI11/libcurl/cpr/nlohmann_json/zstd/libsodium/Catch2/doctest/utf8proc; document in THIRD_PARTY.md.
- No giant unrelated framework, no distributed DB, no custom TLS/compression, no excessive ECS.

## Repository Structure

- CMakeLists.txt primary, Makefile frontend
- CMakePresets.json with debug, release, debug-windows, release-windows, etc.
- include/shadowdeep/ - headers, src/ - implementations
- src/platform/ - only place with platform-specific code
- data/ - content data-driven stable IDs, stays at root (not inside include/src) because it is content, not code. It is installed to share/shadowdeep/data. Do not move into src/include.
- DOCS/ - all markdown docs: BUILDING.md, PORTING.md, THIRD_PARTY.md, CHANGELOG.md, CONTRIBUTING.md, ARCHITECTURE.md, SAVE_FORMAT.md, README.md
- .github/workflows/ - CI, cross, release
- .github/ISSUE_TEMPLATE/ - bug_report, feature_request, port_request
- .github/pull_request_template.md

## Data Folder Location

User asked: shouldn't data folder be inside include or src?

Answer: No. data/ stays at repository root because:
- It is content (monsters, items, regions, quests), not source code
- CMake installs it to share/shadowdeep/data via install(DIRECTORY data/ DESTINATION share/shadowdeep/data)
- Keeping it at root makes it visible, versioned separately, and easy to package into release artifacts shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz
- If it were inside src/, it would mix content with code and break XDG separation (installation vs user data)
- For ports, data/ is read-only and located via getExecutableDir() + /share/shadowdeep/data or via AppPaths

If you need to add new content, add to data/ and reference via stable IDs like monster.ancient_dragon, item.amulet_shadowdeep.

## Building

See DOCS/BUILDING.md for full instructions.

Quick:
```
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

Makefile frontend:
```
make
./build/debug/shadowdeep
```

## CI Monitoring

You must monitor GitHub Actions after each push:

```
gh run list --branch arena/01a0bf69-shadowdeep --limit 5
gh run view <id> --json jobs --jq '.jobs[] | "\(.name) \(.status) \(.conclusion)"'
```

If Windows fails, check issue created by workflow (it posts logs). Common failures:
- CMake Error Unknown argument --verbose on configure preset - only build preset supports --verbose
- dockcross image tag manifest unknown - use :latest not date tag
- Vec2 constexpr manhattan uses std::abs not constexpr in MSVC - use manual abs
- NOMINMAX macro redefinition - guard with #ifndef
- localtime unsafe C4996 - use localtime_s on Windows, localtime_r on POSIX

Fix iteratively until green. Remove continue-on-error once green.

Cross compile uses dockcross/linux-arm64:latest, linux-armv7:latest, linux-riscv64:latest and freebsd VM via vmactions/freebsd-vm.

## Release and Update System

Update function must:
- Detect OS and arch at runtime
- Check GitHub releases via HTTPS only: https://api.github.com/repos/Seigh-sword/shadowdeep/releases/latest
- Parse version, compare with kGameVersion and kBuildRevision
- Download correct artifact: shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz
- Verify size, checksum (SHA256), signature if available
- Never overwrite saves
- Handle Windows self-replace via staged updater in cache/updates/
- Support --check-update and --update CLI flags, work even when non-TTY for info commands

OS detection:
- Windows: _WIN32 -> windows
- Linux: __linux__ -> linux
- macOS: __APPLE__ -> macos
- FreeBSD: __FreeBSD__ -> freebsd
- OpenBSD, NetBSD, etc.

Arch detection:
- x86_64: __x86_64__ or _M_X64
- arm64/aarch64: __aarch64__ or _M_ARM64
- armv7: __arm__ 
- riscv64: __riscv
- ppc64le, etc.

Artifact naming: shadowdeep-Zv1-{os}-{arch}.tar.gz for Unix, .zip for Windows
- windows-x86_64, windows-arm64
- linux-x86_64, linux-aarch64, linux-armv7, linux-riscv64
- linux-arm64-rpi (Raspberry Pi alias for aarch64), linux-armv7-rpi (Raspberry Pi Zero/1)
- macos-arm64, macos-x86_64
- freebsd-x86_64

Release workflow .github/workflows/release.yml must:
- Build Tier1 artifacts
- Build Raspberry Pi artifacts (aarch64 and armv7)
- Generate SHA256 checksums
- Create GitHub release with artifacts via softprops/action-gh-release
- Run on tags Zv1* and v* and manual dispatch, and also nightly if needed

Without releases, --check-update and --update will not work, so ensure releases are published.

## Raspberry Pi Port

Raspberry Pi is Tier2, community-supported, but we now have explicit port:

- OS: Linux
- Arch: aarch64 (Pi 3/4/5 64-bit) and armv7 (Pi Zero/1/2 32-bit)
- Build via dockcross/linux-arm64:latest and linux-armv7:latest
- Requires C++20 compiler, CMake, Ninja
- Tested via QEMU in CI
- Paths use XDG like Linux: ~/.local/share/shadowdeep
- Terminal: works with ANSI, tested with Pi's default terminal, fallback ASCII

Add docs in DOCS/PORTING.md and DOCS/BUILDING.md.

## Badges

README.md should have many badges: CI status, Cross Compile status, Release status, License ISC, Version Zv1, C++20, Platforms (Windows, Linux, macOS, FreeBSD, Raspberry Pi), etc. Use shields.io.

## Porting Intent

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game. Designed for desktop users and CLI-based distros and OSes. We are always open to PRs for custom ports, will fix bugs ourselves if requested, and accept Issues for port requests. See DOCS/PORTING.md and .github/ISSUE_TEMPLATE/port_request.md.

## Workflow

1. Make changes
2. Build locally if possible
3. Commit and push to arena/01a0bf69-shadowdeep
4. Monitor gh run list
5. Fix failures iteratively
6. Do not create PRs, push directly to arena branch per Arena instructions
7. Keep version Zv1 constant
8. Document in DOCS/

## Contact

- Author: github.com/Seigh-sword
- Repo: https://github.com/Seigh-sword/shadowdeep
