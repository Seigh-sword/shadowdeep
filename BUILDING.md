# Building SHADOWDEEP

## Requirements

- C++20 compiler: GCC 11+, Clang 14+, MSVC 2022+
- CMake 3.20+
- Ninja recommended
- No mandatory external dependencies for core build

Optional:

- FTXUI (via FetchContent) for enhanced TUI
- nlohmann/json for config (header-only, vendored or FetchContent)
- cpr + libcurl for update checking
- zstd for save compression
- Catch2 for tests

## Quick Build

```bash
cmake --preset debug
cmake --build --preset debug
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
cmake --preset release
cmake --build --preset release
.\build\release\shadowdeep.exe --version
```

Requires Visual Studio 2022 with C++ workload.

## Linux

Ubuntu/Debian:

```bash
sudo apt install build-essential cmake ninja-build
cmake --preset debug
cmake --build --preset debug
```

## macOS

```bash
brew install cmake ninja
cmake --preset debug
cmake --build --preset debug
```

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

See .github/workflows for examples.

Linux AArch64:

```bash
docker run --rm -v $PWD:/work dockcross/linux-arm64 cmake -B /work/build/aarch64
```

## Install

```bash
cmake --install build/release
```
