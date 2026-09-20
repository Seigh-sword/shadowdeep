#include "shadowdeep/config/config.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace shadowdeep {

namespace fs = std::filesystem;

ConfigManager::ConfigManager(const AppPaths& paths) : paths_(paths) {
    setDefaults();
}

bool ConfigManager::load() {
    try {
        if (!fs::exists(paths_.configFile)) {
            setDefaults();
            return true;
        }
        std::ifstream f(paths_.configFile);
        if (!f) return false;
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        auto findValue = [&](const std::string& key, std::string& out) -> bool {
            std::string search = "\"" + key + "\"";
            size_t pos = content.find(search);
            if (pos == std::string::npos) return false;
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) return false;
            size_t q1 = content.find('"', colon + 1);
            if (q1 == std::string::npos) return false;
            size_t q2 = content.find('"', q1 + 1);
            if (q2 == std::string::npos) return false;
            out = content.substr(q1 + 1, q2 - q1 - 1);
            return true;
        };

        auto findBool = [&](const std::string& key, bool& out) -> bool {
            std::string search = "\"" + key + "\"";
            size_t pos = content.find(search);
            if (pos == std::string::npos) return false;
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) return false;
            size_t v = content.find_first_not_of(" \t\n\r", colon + 1);
            if (v == std::string::npos) return false;
            if (content.compare(v, 4, "true") == 0) { out = true; return true; }
            if (content.compare(v, 5, "false") == 0) { out = false; return true; }
            return false;
        };

        auto findInt = [&](const std::string& key, int& out) -> bool {
            std::string search = "\"" + key + "\"";
            size_t pos = content.find(search);
            if (pos == std::string::npos) return false;
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) return false;
            size_t v = content.find_first_of("-0123456789", colon + 1);
            if (v == std::string::npos) return false;
            size_t end = content.find_first_not_of("0123456789", v + (content[v] == '-' ? 1 : 0));
            std::string num = content.substr(v, end - v);
            try { out = std::stoi(num); return true; } catch (...) { return false; }
        };

        std::string tmp;
        bool btmp;
        int itmp;

        if (findValue("theme", tmp)) config_.theme = tmp;
        if (findBool("animations", btmp)) config_.animations = btmp;
        if (findBool("reducedMotion", btmp)) config_.reducedMotion = btmp;
        if (findBool("screenShake", btmp)) config_.screenShake = btmp;
        if (findBool("highContrast", btmp)) config_.highContrast = btmp;
        if (findBool("mouseSupport", btmp)) config_.mouseSupport = btmp;
        if (findBool("autoPickupGold", btmp)) config_.autoPickupGold = btmp;
        if (findBool("autoCheckUpdate", btmp)) config_.autoCheckUpdate = btmp;
        if (findInt("animationSpeed", itmp)) config_.animationSpeed = itmp;
        if (findValue("updateChannel", tmp)) config_.updateChannel = tmp;
        if (findValue("lastEntryId", tmp)) config_.lastEntryId = tmp;

        return true;
    } catch (...) {
        setDefaults();
        return false;
    }
}

bool ConfigManager::save() {
    try {
        fs::create_directories(paths_.configDir);
        std::string tmpPath = paths_.configFile.string() + ".tmp";
        std::ofstream f(tmpPath);
        if (!f) return false;

        f << "{\n";
        f << "  \"schemaVersion\": " << config_.schemaVersion << ",\n";
        f << "  \"theme\": \"" << config_.theme << "\",\n";
        f << "  \"animations\": " << (config_.animations ? "true" : "false") << ",\n";
        f << "  \"animationSpeed\": " << config_.animationSpeed << ",\n";
        f << "  \"reducedMotion\": " << (config_.reducedMotion ? "true" : "false") << ",\n";
        f << "  \"screenShake\": " << (config_.screenShake ? "true" : "false") << ",\n";
        f << "  \"highContrast\": " << (config_.highContrast ? "true" : "false") << ",\n";
        f << "  \"mouseSupport\": " << (config_.mouseSupport ? "true" : "false") << ",\n";
        f << "  \"autoPickupGold\": " << (config_.autoPickupGold ? "true" : "false") << ",\n";
        f << "  \"autoCheckUpdate\": " << (config_.autoCheckUpdate ? "true" : "false") << ",\n";
        f << "  \"updateChannel\": \"" << config_.updateChannel << "\",\n";
        f << "  \"lastEntryId\": \"" << config_.lastEntryId << "\"\n";
        f << "}\n";
        f.flush();
        f.close();
        fs::rename(fs::path(tmpPath), paths_.configFile);
        return true;
    } catch (...) {
        return false;
    }
}

Config& ConfigManager::get() {
    return config_;
}

const Config& ConfigManager::get() const {
    return config_;
}

void ConfigManager::setDefaults() {
    config_ = Config{};
}

}
