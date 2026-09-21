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
- cpr + libcurl for update checking (--check-update/--update) - auto-detected, HTTPS only
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

Update uses WinHTTP (built-in), no extra deps, HTTPS verified.

## Linux

Ubuntu/Debian:

```bash
sudo apt install build-essential cmake ninja-build libcurl4-openssl-dev
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

XDG compliant: data in `$XDG_DATA_HOME/shadowdeep` or `~/.local/share/shadowdeep`.

Update uses libcurl if found, else falls back to curl binary, HTTPS verified, checksum SHA256.

## macOS

```bash
brew install cmake ninja curl
cmake --preset debug
cmake --build --preset debug
./build/debug/shadowdeep --version
```

Paths: `~/Library/Application Support/shadowdeep/`

## Raspberry Pi

Raspberry Pi is Tier2 officially supported, built via same Linux code path.

### Pi 3/4/5 64-bit (aarch64)

On Pi directly:

```bash
sudo apt install build-essential cmake ninja-build libcurl4-openssl-dev
cmake --preset rpi-aarch64
cmake --build --preset rpi-aarch64
./build/rpi-aarch64/shadowdeep --version
```

Cross compile from x86_64:

```bash
docker pull dockcross/linux-arm64:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake -B build/rpi-aarch64 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF -DSHADOWDEEP_RPI=ON
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake --build build/rpi-aarch64
```

Artifact: `shadowdeep-Zv1-raspberrypi-aarch64.tar.gz` also aliased as `shadowdeep-Zv1-linux-aarch64.tar.gz` and `shadowdeep-Zv1-linux-arm64.tar.gz`

### Pi Zero/1/2 32-bit (armv7)

On Pi directly:

```bash
sudo apt install build-essential cmake ninja-build
cmake --preset rpi-armv7
cmake --build --preset rpi-armv7
./build/rpi-armv7/shadowdeep --version
```

Cross compile:

```bash
docker pull dockcross/linux-armv7:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake -B build/rpi-armv7 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF -DSHADOWDEEP_RPI=ON
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake --build build/rpi-armv7
```

Artifact: `shadowdeep-Zv1-raspberrypi-armv7.tar.gz` also `shadowdeep-Zv1-linux-armv7.tar.gz` and `shadowdeep-Zv1-linux-armv7-rpi.tar.gz`

### Running on Pi

```bash
tar xzf shadowdeep-Zv1-raspberrypi-aarch64.tar.gz
./shadowdeep/shadowdeep --version
./shadowdeep/shadowdeep --help
./shadowdeep/shadowdeep
```

Pi uses XDG paths like Linux: `~/.local/share/shadowdeep/saves/`

Terminal: works with Pi default terminal, LXTerminal, fallback ASCII if needed.

Update: `shadowdeep --check-update` detects `linux-aarch64` or `linux-armv7` and downloads correct artifact, verifies SHA256, stages to `~/.cache/shadowdeep/updates/`.

### CI for Pi

See `.github/workflows/cross.yml` raspberrypi job and `.github/workflows/release.yml` build-cross job. Both use dockcross and QEMU, built on every push to arena/* and main.

## BSDs

FreeBSD:

```bash
pkg install cmake ninja curl
cmake --preset debug
cmake --build --preset debug
```

OpenBSD/NetBSD: similar, may need `cmake -B build/debug -G Ninja`.

FreeBSD artifact: `shadowdeep-Zv1-freebsd-x86_64.tar.gz`

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

See .github/workflows/cross.yml and release.yml for examples.

Linux AArch64 / ARMv7 / RISC-V64 / Raspberry Pi using dockcross:

```bash
docker pull dockcross/linux-arm64:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake -B /work/build/aarch64 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF
docker run --rm -v $PWD:/work -w /work dockcross/linux-arm64:latest cmake --build /work/build/aarch64

docker pull dockcross/linux-armv7:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake -B /work/build/armv7 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF
docker run --rm -v $PWD:/work -w /work dockcross/linux-armv7:latest cmake --build /work/build/armv7

docker pull dockcross/linux-riscv64:latest
docker run --rm -v $PWD:/work -w /work dockcross/linux-riscv64:latest cmake -B /work/build/riscv64 -DCMAKE_BUILD_TYPE=Release -DSHADOWDEEP_BUILD_TESTS=OFF
docker run --rm -v $PWD:/work -w /work dockcross/linux-riscv64:latest cmake --build /work/build/riscv64
```

FreeBSD via VM is used in CI.

Raspberry Pi is same as Linux aarch64/armv7 but with `-DSHADOWDEEP_RPI=ON` for docs.

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

Data folder stays at repo root `data/` (not inside `src/` or `include/`) because it is content, not code. It is installed to `share/shadowdeep/data`. See AGENTS.md.

## Artifacts

Release artifacts are named `shadowdeep-Zv1-{os}-{arch}.zip/.tar.gz` and include LICENSE, README, AGENTS.md, DOCS/, data/.

List:

- windows-x86_64.zip, windows-arm64.zip
- linux-x86_64.tar.gz, linux-aarch64.tar.gz, linux-arm64.tar.gz, linux-armv7.tar.gz, linux-riscv64.tar.gz
- raspberrypi-aarch64.tar.gz, raspberrypi-armv7.tar.gz, linux-arm64-rpi.tar.gz, linux-armv7-rpi.tar.gz
- macos-arm64.tar.gz, macos-x86_64.tar.gz
- freebsd-x86_64.tar.gz

Each has .sha256 checksum and SHA256SUMS file in release.

Update manager detects OS/arch and downloads correct one via HTTPS, verifies size and SHA256.
