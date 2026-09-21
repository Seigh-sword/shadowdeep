#pragma once
#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace shadowdeep {

struct UpdateAsset {
    std::string name;
    std::string url;
    int64_t size = 0;
    std::string contentType;
};

struct UpdateInfo {
    std::string version;
    std::string tag;
    std::string url;
    std::string notes;
    int64_t size = 0;
    std::string sha256;
    bool available = false;
    std::string os;
    std::string arch;
    std::string artifactName;
    std::vector<UpdateAsset> assets;
    std::string downloadUrl;
    std::string checksumUrl;
};

class UpdateManager {
public:
    static std::string getOS();
    static std::string getArch();
    static std::string getOSDisplay();
    static std::string getArchDisplay();
    static std::vector<std::string> getCandidateArtifactNames();
    static std::string getCurrentPlatformString();
    static std::string getDirectDownloadUrl(const std::string& version, const std::string& artifactName = "");
    static std::string getDirectDownloadUrlForCurrent(const std::string& version);
    static std::string getCurrentExecutablePath();
    static bool isExecutableWritable(const std::string& path);

    std::optional<UpdateInfo> checkForUpdate(const std::string& currentVersion);
    bool downloadUpdate(const UpdateInfo& info, const std::string& destPath);
    bool verifyChecksum(const std::string& filePath, const std::string& expectedSha256);
    bool verifySize(const std::string& filePath, int64_t expectedSize);
    bool attemptAutoInstall(const std::string& downloadedPath, const UpdateInfo& info);

    static std::string computeSHA256(const std::string& filePath);
    static std::string getLatestReleaseApiUrl();
    static std::string getReleasesPageUrl();

private:
    static std::string httpGet(const std::string& url);
    static bool httpDownload(const std::string& url, const std::string& destPath);
    static std::optional<UpdateInfo> parseReleaseJson(const std::string& json, const std::string& os, const std::string& arch);
    static std::string toLower(std::string s);
    static std::string trim(const std::string& s);
    static bool extractAndReplace(const std::string& archivePath, const std::string& exePath);
};

}
