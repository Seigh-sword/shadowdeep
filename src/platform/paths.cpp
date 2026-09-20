#include "shadowdeep/platform/paths.hpp"
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

namespace shadowdeep {

namespace fs = std::filesystem;

static fs::path homeDir() {
#ifdef _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) return fs::path(userProfile);
    return fs::path("C:\\");
#else
    const char* home = std::getenv("HOME");
    if (home) return fs::path(home);
    struct passwd* pw = getpwuid(getuid());
    if (pw) return fs::path(pw->pw_dir);
    return fs::path("/tmp");
#endif
}

static fs::path xdgOrHome(const char* envName, const fs::path& fallbackFromHome) {
#ifdef _WIN32
    (void)envName;
    (void)fallbackFromHome;
    return fs::path();
#else
    const char* v = std::getenv(envName);
    if (v && *v) return fs::path(v);
    return homeDir() / fallbackFromHome;
#endif
}

AppPaths getAppPaths() {
    AppPaths p;

#ifdef _WIN32
    fs::path appData;
    PWSTR wpath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &wpath))) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, nullptr, 0, nullptr, nullptr);
        if (len > 0) {
            std::string utf8(len - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, wpath, -1, utf8.data(), len, nullptr, nullptr);
            appData = fs::path(utf8);
        }
        CoTaskMemFree(wpath);
    }
    if (appData.empty()) {
        const char* ap = std::getenv("APPDATA");
        if (ap) appData = fs::path(ap);
        else appData = homeDir() / "AppData" / "Roaming";
    }
    p.dataRoot = appData / "shadowdeep";
    p.configRoot = p.dataRoot;
    p.cacheRoot = appData / "shadowdeep" / "cache";
    p.stateRoot = p.dataRoot;
#else
    fs::path dataHome = xdgOrHome("XDG_DATA_HOME", fs::path(".local/share"));
    fs::path configHome = xdgOrHome("XDG_CONFIG_HOME", fs::path(".config"));
    fs::path cacheHome = xdgOrHome("XDG_CACHE_HOME", fs::path(".cache"));
    fs::path stateHome = xdgOrHome("XDG_STATE_HOME", fs::path(".local/state"));

    p.dataRoot = dataHome / "shadowdeep";
    p.configRoot = configHome / "shadowdeep";
    p.cacheRoot = cacheHome / "shadowdeep";
    p.stateRoot = stateHome / "shadowdeep";
    if (std::getenv("XDG_STATE_HOME") == nullptr) {
        p.stateRoot = p.dataRoot;
    }
#endif

    p.savesDir = p.dataRoot / "saves";
    p.backupsDir = p.dataRoot / "save_backups";
    p.configDir = p.configRoot;
    p.logsDir = p.dataRoot / "logs";
    p.cacheDir = p.cacheRoot;
    p.updatesDir = p.cacheRoot / "updates";
    p.exportsDir = p.dataRoot / "exports";
    p.runHistoryDir = p.dataRoot / "run_history";
    p.crashReportsDir = p.dataRoot / "crash_reports";

    p.configFile = p.configDir / "config.json";
    p.logFile = p.logsDir / "shadowdeep.log";

    return p;
}

bool ensureAppDirs(const AppPaths& p) {
    try {
        fs::create_directories(p.savesDir);
        fs::create_directories(p.backupsDir);
        fs::create_directories(p.configDir);
        fs::create_directories(p.logsDir);
        fs::create_directories(p.cacheDir);
        fs::create_directories(p.updatesDir);
        fs::create_directories(p.exportsDir);
        fs::create_directories(p.runHistoryDir);
        fs::create_directories(p.crashReportsDir);
        return true;
    } catch (...) {
        return false;
    }
}

fs::path getExecutableDir() {
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len == 0) return fs::current_path();
    fs::path full(buf);
    return full.parent_path();
#else
    try {
        fs::path exe = fs::read_symlink("/proc/self/exe");
        return exe.parent_path();
    } catch (...) {
        return fs::current_path();
    }
#endif
}

fs::path getExecutablePath() {
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len == 0) return fs::path();
    return fs::path(buf);
#else
    try {
        return fs::read_symlink("/proc/self/exe");
    } catch (...) {
        return fs::path();
    }
#endif
}

}
