#include "shadowdeep/update/update_manager.hpp"

namespace shadowdeep {

std::optional<UpdateInfo> UpdateManager::checkForUpdate(const std::string& currentVersion) {
    (void)currentVersion;
    return std::nullopt;
}

bool UpdateManager::downloadUpdate(const UpdateInfo& info, const std::string& destPath) {
    (void)info;
    (void)destPath;
    return false;
}

bool UpdateManager::verifyChecksum(const std::string& filePath, const std::string& expectedSha256) {
    (void)filePath;
    (void)expectedSha256;
    return false;
}

}
