#pragma once
#include <filesystem>
#include <optional>

namespace shadowdeep {

struct AppPaths {
    std::filesystem::path dataRoot;
    std::filesystem::path configRoot;
    std::filesystem::path cacheRoot;
    std::filesystem::path stateRoot;

    std::filesystem::path savesDir;
    std::filesystem::path backupsDir;
    std::filesystem::path configDir;
    std::filesystem::path logsDir;
    std::filesystem::path cacheDir;
    std::filesystem::path updatesDir;
    std::filesystem::path exportsDir;
    std::filesystem::path runHistoryDir;
    std::filesystem::path crashReportsDir;

    std::filesystem::path configFile;
    std::filesystem::path logFile;
};

AppPaths getAppPaths();
bool ensureAppDirs(const AppPaths& p);
std::filesystem::path getExecutableDir();
std::filesystem::path getExecutablePath();

}
