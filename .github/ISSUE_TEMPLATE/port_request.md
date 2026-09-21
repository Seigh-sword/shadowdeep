---
name: Port Request
about: Request SHADOWDEEP to be ported to a new OS or architecture
title: "[PORT] "
labels: port-request
assignees: ""
---

## Target Platform

- OS: [e.g., OpenBSD, NetBSD, DragonFly BSD, TempleOS research, Haiku, SerenityOS]
- Architecture: [e.g., x86_64, aarch64, riscv64, ppc64le]
- Distribution if relevant: [e.g., Ubuntu, Fedora, Alpine]

## Why This Port Matters

Why would this port be useful? Who would use it?

## Current Status

- Have you tried building on this platform? What happened?
- Does the platform have a C++20 compiler?
- Does it have CMake?
- Does it have a usable terminal? (ANSI, VT, etc.)

## Willingness to Contribute

- [ ] I can test builds on this platform
- [ ] I can provide a PR with initial port code
- [ ] I can provide access to hardware/VM for CI
- [ ] I just want to request it and see if someone else can do it

## Porting Notes

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a game.

We welcome custom ports:

- You can create a PR for your own version or port
- If the PR's CI fails, we will help fix the bugs ourselves if you request it
- You can open this issue requesting a custom port

See DOCS/PORTING.md for what a new backend needs to implement:

- ITerminalBackend: enterRaw, restore, getSize, pollKey, waitKey, writeRaw, hideCursor, showCursor, clearScreen
- PlatformPaths: getAppPaths, ensureAppDirs, getExecutableDir

If the platform cannot support FTXUI, you can implement a minimal ANSI renderer that satisfies the same interface.

## Additional Context

Add any other context about the platform, toolchain, or constraints.
