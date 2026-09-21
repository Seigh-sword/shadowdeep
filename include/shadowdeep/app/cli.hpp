#pragma once
#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace shadowdeep {

struct CliOptions {
    bool showHelp = false;
    bool showVersion = false;
    bool showAbout = false;
    bool showLicense = false;
    bool checkUpdate = false;
    bool doUpdate = false;
    bool autoUpdate = false;
    bool runTests = false;
    std::optional<uint32_t> seed;
    std::string entryToLoad;
    std::vector<std::string> extraArgs;
};

CliOptions parseCli(int argc, char** argv);
void printHelp(const char* exe);
void printVersion();
void printAbout();
void printLicense();

}
