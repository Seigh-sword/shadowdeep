# Pull Request

## Description

What does this PR do? Explain clearly.

## Related Issues

Fixes # (issue)

## Type

- [ ] Bug fix
- [ ] Feature
- [ ] Port to new OS/architecture
- [ ] Documentation
- [ ] CI / Build
- [ ] Refactor

## Checklist

- [ ] I have read DOCS/CONTRIBUTING.md
- [ ] I did NOT increment Zv1 version (only owner decides version)
- [ ] I did NOT add emojis to code or public docs unless requested
- [ ] I did NOT add comments to .cpp/.hpp files (per project rule)
- [ ] My code is C++20 or newer, no raw owning pointers where RAII works
- [ ] I respected platform abstraction (no `#ifdef _WIN32` in gameplay)
- [ ] I tested on at least one Tier1 platform or explained testing
- [ ] I added tests where applicable
- [ ] I updated documentation in DOCS/ where needed
- [ ] Save compatibility considered (stable IDs, migration if needed)

## Porting PRs

If this is a port:

- [ ] I implemented ITerminalBackend for my platform
- [ ] I implemented PlatformPaths correctly (XDG, AppData, etc.)
- [ ] I documented build steps in DOCS/BUILDING.md if needed
- [ ] I documented porting notes in DOCS/PORTING.md if needed
- [ ] I understand that if CI fails, maintainers will help fix bugs if I request it
- [ ] I tested `shadowdeep --version` outputs Zv1 and `shadowdeep --help` works

## Screenshots / Terminal Output

If applicable, add text-based output or describe TUI changes.

## Additional Notes

Anything else?
