# Contributing to SHADOWDEEP

## Versioning

Current public version is **Zv1** (Zero v1).

- Do NOT increment Zv1 to Zv2, 0.1.0, 1.0.0 etc.
- Only project owner decides when Zv1 is complete.
- Save schema, config schema, build revision MAY change independently while game version remains Zv1.

## Code Style

- C++20 or newer
- No comments in code (`//` or `/* */` forbidden in .cpp/.hpp)
- Clear names, small cohesive components, good file organization
- RAII, no raw owning pointers
- Use std::unique_ptr, std::vector, std::string, std::optional, etc.
- No emojis in code or commits (except GitHub templates where appropriate)

## Architecture

- No `#ifdef _WIN32` in gameplay code
- Platform code only in `src/platform/` and `include/shadowdeep/platform/`
- Keep installation and user data separate
- Use fixed-width integers for saves, no raw struct dumps

## Terminology

- Project name: SHADOWDEEP
- Save menu: Entries (not Games, Adventures, Profiles)
- Internal code: SaveFile, SaveManager, SaveMetadata, EntryMetadata

## Building

See DOCS/BUILDING.md

## Porting Intent

SHADOWDEEP was intentionally made for porting and to run anywhere while also being a real game.

We are always open to ports:

- Create a PR for your own version or port
- If CI fails, we will fix bugs ourselves if you request it
- Or open an Issue with label `port-request`

See DOCS/PORTING.md for details, and `.github/ISSUE_TEMPLATE/port_request.md` for the template.

## Tests

- Add tests for new systems
- Run `ctest --preset debug`

## Pull Requests

- Do not include age in credits
- Keep attribution: github.com/Seigh-sword
- Repository: https://github.com/Seigh-sword/shadowdeep
- ISC License must be kept
- Use `.github/pull_request_template.md`
- If your PR is a port and fails CI, add a comment asking for help - maintainers will help fix it

## Issues

- Bug reports: `.github/ISSUE_TEMPLATE/bug_report.md`
- Feature requests: `.github/ISSUE_TEMPLATE/feature_request.md`
- Port requests: `.github/ISSUE_TEMPLATE/port_request.md` - request SHADOWDEEP on your OS

## Communication

- Explain architectural decisions clearly
- If request creates security/portability problem, explain and propose better implementation
- Be respectful: this project aims to be welcoming for CLI and desktop users alike
