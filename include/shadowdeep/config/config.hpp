#pragma once
#include <string>
#include <filesystem>
#include "../platform/paths.hpp"

namespace shadowdeep {

enum class ColorMode : uint8_t {
    Auto = 0,
    TrueColor = 1,
    Color256 = 2,
    Color16 = 3,
    NoColor = 4
};

enum class UnicodeMode : uint8_t {
    Auto = 0,
    Unicode = 1,
    Ascii = 2
};

struct Config {
    uint16_t schemaVersion = 4;
    ColorMode colorMode = ColorMode::Auto;
    UnicodeMode unicodeMode = UnicodeMode::Auto;
    std::string theme = "dark";
    bool animations = true;
    int animationSpeed = 100;
    bool reducedMotion = false;
    bool screenShake = true;
    bool highContrast = false;
    bool mouseSupport = false;
    bool autoPickupGold = true;
    bool autoPickupRubies = true;
    bool confirmDelete = true;
    bool compactUi = false;
    bool autoCheckUpdate = false;
    std::string updateChannel = "stable";
    int autosaveIntervalMinutes = 5;
    std::string lastEntryId;
    int messageVerbosity = 2;
};

class ConfigManager {
public:
    explicit ConfigManager(const AppPaths& paths);

    bool load();
    bool save();
    Config& get();
    const Config& get() const;

    void setDefaults();

private:
    AppPaths paths_;
    Config config_;
};

}
