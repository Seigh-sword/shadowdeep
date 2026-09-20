#pragma once
#include <string>
#include <optional>

namespace shadowdeep {

struct UpdateInfo {
    std::string version;
    std::string url;
    std::string notes;
    int64_t size = 0;
    std::string sha256;
    bool available = false;
};

class UpdateManager {
public:
    std::optional<UpdateInfo> checkForUpdate(const std::string& currentVersion);
    bool downloadUpdate(const UpdateInfo& info, const std::string& destPath);
    bool verifyChecksum(const std::string& filePath, const std::string& expectedSha256);
};

}
