# Building SHADOWDEEP

SHADOWDEEP Zv1 is intentionally built for porting. It builds anywhere with a C++20 compiler and CMake.

## Requirements

- C++20 compiler: GCC 11+, Clang 14+, MSVC 2022+
- CMake 3.20+
- Ninja recommended, but Make works
- No mandatory external dependencies for core build (fallback ANSI renderer, internal mini JSON)

Optional (auto-detected or FetchContent):

- FTXUI (MIT) via FetchContent for enhanced TUI when `SHADOWDEEP_USE_FTXUI=ON`
- nlohmann/json for config (header-only)
- cpr + libcurl for update checking (--check-update/--update)
- zstd for save compression
- Catch2 for tests

## Quick Build

```bash
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

Release:

```bash
cmake --preset release
cmake --build --preset release
```

Using Makefile frontend:

```bash
make
make debug
make release
make test
make clean
```

## Windows

```powershell
cmake --preset debug-windows
cmake --build --preset debug-windows
.\build\debug-windows\shadowdeep.exe --version
```

Requires Visual Studio 2022 with C++ workload. Also builds with Clang-CL.

Paths: `%APPDATA%\shadowdeep\` for user data.

## Linux

Ubuntu/Debian:

```bash
sudo apt install build-essential cmake ninja-build
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

XDG compliant: data in `$XDG_DATA_HOME/shadowdeep` or `~/.local/share/shadowdeep`.

## macOS

```bash
brew install cmake ninja
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

Paths: `~/Library/Application Support/shadowdeep/`

## BSDs

FreeBSD:

```bash
pkg install cmake ninja
cmake --preset debug
cmake --build --preset debug
```

OpenBSD/NetBSD: similar, may need `cmake -B build/debug -G Ninja`.

## Tests

```bash
ctest --preset debug --output-on-failure
```

## Sanitizers

```bash
cmake -B build/asan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
cmake --build build/asan
```

## Cross Compilation

See .github/workflows/cross.yml for examples.

Linux AArch64 / ARMv7 / RISC-V64 using dockcross:

```bash
docker pull dockcross/linux-arm64:latest
docker run --rm -v $PWD:/work dockcross/linux-arm64:latest cmake -B /work/build/aarch64 -DCMAKE_BUILD_TYPE=Release
docker run --rm -v $PWD:/work dockcross/linux-arm64:latest cmake --build /work/build/aarch64
```

FreeBSD via VM is used in CI.

## Custom Ports

If you are porting to a new OS, see DOCS/PORTING.md.

SHADOWDEEP was intentionally made to run anywhere while also being a real game. If you create a port PR and CI fails, we will fix bugs ourselves if you request it. You can also open an Issue with label `port-request`.

Steps for new OS:

1. Implement `ITerminalBackend` and `PlatformPaths` in `src/platform/<your_os>/`
2. Add to CMakeLists.txt
3. Build with `cmake --preset debug`
4. Verify `--version` prints Zv1

## Install

```bash
cmake --install build/release
```

Install keeps user data separate: binaries go to prefix, saves stay in AppData/XDG/Application Support.

## Artifacts

Release artifacts are named `shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz` and include LICENSE and DOCS/.
