# Third Party Licenses

SHADOWDEEP is ISC licensed. Third-party dependencies have their own licenses.

## Core (Optional)

- FTXUI - https://github.com/ArthurSonzogni/FTXUI - MIT License
  - Functional Terminal User Interface, used for enhanced TUI when enabled

- fmt - https://github.com/fmtlib/fmt - MIT License
  - Formatting library, optional, fallback to std::format

- spdlog - https://github.com/gabime/spdlog - MIT License
  - Logging, optional

- CLI11 - https://github.com/CLIUtils/CLI11 - BSD-3-Clause
  - Command-line parsing, header-only

- nlohmann/json - https://github.com/nlohmann/json - MIT License
  - JSON for config and update metadata

- cpr - https://github.com/libcpr/cpr - MIT License
  - C++ Requests, wraps libcurl for HTTPS

- libcurl - https://curl.se/ - curl License (MIT-like)
  - HTTP client

- zstd - https://github.com/facebook/zstd - BSD License
  - Compression for saves

- Catch2 - https://github.com/catchorg/Catch2 - BSL-1.0
  - Testing framework

## Notes

- All chosen libraries have ISC-compatible licenses (MIT, BSD, BSL)
- No GPL code linked in release builds
- Vendored headers include their license headers
- Release artifacts include this file
