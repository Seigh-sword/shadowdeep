#include "shadowdeep/update/update_manager.hpp"
#include "shadowdeep/version.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <unistd.h>
#endif

#if __has_include(<curl/curl.h>)
#include <curl/curl.h>
#define SHADOWDEEP_HAS_CURL 1
#endif

namespace shadowdeep {

namespace fs = std::filesystem;

std::string UpdateManager::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string UpdateManager::trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b-1]))) --b;
    return s.substr(a, b-a);
}

std::string UpdateManager::getOS() {
#ifdef _WIN32
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__FreeBSD__)
    return "freebsd";
#elif defined(__OpenBSD__)
    return "openbsd";
#elif defined(__NetBSD__)
    return "netbsd";
#elif defined(__linux__)
#if defined(__arm__) || defined(__aarch64__)
    return "linux";
#else
    return "linux";
#endif
#else
    return "linux";
#endif
}

std::string UpdateManager::getArch() {
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
    return "x86_64";
#elif defined(_M_ARM64) || defined(__aarch64__)
    return "aarch64";
#elif defined(__arm__) || defined(_M_ARM)
#if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    return "armv7";
#else
    return "armv7";
#endif
#elif defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
    return "riscv64";
#elif defined(__ppc64__) || defined(__powerpc64__)
    return "ppc64le";
#else
    return "x86_64";
#endif
}

std::string UpdateManager::getOSDisplay() {
    std::string os = getOS();
    if (os == "windows") return "Windows";
    if (os == "macos") return "macOS";
    if (os == "linux") return "Linux";
    if (os == "freebsd") return "FreeBSD";
    if (os == "openbsd") return "OpenBSD";
    if (os == "netbsd") return "NetBSD";
    return os;
}

std::string UpdateManager::getArchDisplay() {
    std::string arch = getArch();
    if (arch == "x86_64") return "x86_64";
    if (arch == "aarch64") return "ARM64";
    if (arch == "armv7") return "ARMv7";
    if (arch == "riscv64") return "RISC-V 64";
    return arch;
}

std::string UpdateManager::getCurrentPlatformString() {
    return getOS() + "-" + getArch();
}

std::vector<std::string> UpdateManager::getCandidateArtifactNames() {
    std::string os = getOS();
    std::string arch = getArch();
    std::vector<std::string> names;

    if (os == "windows") {
        names.push_back("shadowdeep-Zv1-" + os + "-" + arch + ".zip");
        if (arch == "aarch64") {
            names.push_back("shadowdeep-Zv1-windows-arm64.zip");
            names.push_back("shadowdeep-Zv1-windows-aarch64.zip");
        }
        if (arch == "x86_64") {
            names.push_back("shadowdeep-Zv1-windows-x86_64.zip");
        }
    } else {
        names.push_back("shadowdeep-Zv1-" + os + "-" + arch + ".tar.gz");
        if (os == "linux") {
            if (arch == "aarch64") {
                names.push_back("shadowdeep-Zv1-linux-arm64.tar.gz");
                names.push_back("shadowdeep-Zv1-linux-aarch64.tar.gz");
                names.push_back("shadowdeep-Zv1-raspberrypi-aarch64.tar.gz");
                names.push_back("shadowdeep-Zv1-linux-arm64-rpi.tar.gz");
            }
            if (arch == "armv7") {
                names.push_back("shadowdeep-Zv1-linux-armv7.tar.gz");
                names.push_back("shadowdeep-Zv1-raspberrypi-armv7.tar.gz");
                names.push_back("shadowdeep-Zv1-linux-armv7-rpi.tar.gz");
            }
            if (arch == "x86_64") {
                names.push_back("shadowdeep-Zv1-linux-x86_64.tar.gz");
            }
            if (arch == "riscv64") {
                names.push_back("shadowdeep-Zv1-linux-riscv64.tar.gz");
            }
        }
        if (os == "macos") {
            if (arch == "aarch64") {
                names.push_back("shadowdeep-Zv1-macos-arm64.tar.gz");
                names.push_back("shadowdeep-Zv1-macos-aarch64.tar.gz");
            }
            if (arch == "x86_64") {
                names.push_back("shadowdeep-Zv1-macos-x86_64.tar.gz");
            }
        }
        if (os == "freebsd") {
            names.push_back("shadowdeep-Zv1-freebsd-x86_64.tar.gz");
            names.push_back("shadowdeep-Zv1-freebsd-aarch64.tar.gz");
        }
    }

    return names;
}

std::string UpdateManager::getLatestReleaseApiUrl() {
    return "https://api.github.com/repos/Seigh-sword/shadowdeep/releases/latest";
}

std::string UpdateManager::getReleasesPageUrl() {
    return "https://github.com/Seigh-sword/shadowdeep/releases";
}

#ifdef SHADOWDEEP_HAS_CURL
static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

static size_t curlWriteFileCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* f = static_cast<std::ofstream*>(stream);
    f->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
#endif

std::string UpdateManager::httpGet(const std::string& url) {
#ifdef _WIN32
    std::string result;
    std::wstring wurl(url.begin(), url.end());

    std::wstring host;
    std::wstring path;
    bool https = false;

    std::string lowerUrl = toLower(url);
    if (lowerUrl.rfind("https://", 0) == 0) {
        https = true;
        std::string rest = url.substr(8);
        size_t slash = rest.find('/');
        if (slash == std::string::npos) {
            host = std::wstring(rest.begin(), rest.end());
            path = L"/";
        } else {
            host = std::wstring(rest.substr(0, slash).begin(), rest.substr(0, slash).end());
            path = std::wstring(rest.substr(slash).begin(), rest.substr(slash).end());
        }
    } else if (lowerUrl.rfind("http://", 0) == 0) {
        https = false;
        std::string rest = url.substr(7);
        size_t slash = rest.find('/');
        if (slash == std::string::npos) {
            host = std::wstring(rest.begin(), rest.end());
            path = L"/";
        } else {
            host = std::wstring(rest.substr(0, slash).begin(), rest.substr(0, slash).end());
            path = std::wstring(rest.substr(slash).begin(), rest.substr(slash).end());
        }
    } else {
        return "";
    }

    HINTERNET hSession = WinHttpOpen(L"SHADOWDEEP/" L"Zv1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    INTERNET_PORT port = https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "";
    }

    DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    WinHttpAddRequestHeaders(hRequest, L"User-Agent: SHADOWDEEP-Zv1\r\n", -1, WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpAddRequestHeaders(hRequest, L"Accept: application/vnd.github.v3+json\r\n", -1, WINHTTP_ADDREQ_FLAG_ADD);

    BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (bResults) bResults = WinHttpReceiveResponse(hRequest, nullptr);

    if (bResults) {
        DWORD dwSize = 0;
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            std::vector<char> buf(dwSize);
            DWORD dwDownloaded = 0;
            if (!WinHttpReadData(hRequest, buf.data(), dwSize, &dwDownloaded)) break;
            result.append(buf.data(), dwDownloaded);
        } while (dwSize > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return result;
#else
#ifdef SHADOWDEEP_HAS_CURL
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "SHADOWDEEP-Zv1");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return "";
    return response;
#else
    std::string cmd = "curl -L -s -A \"SHADOWDEEP-Zv1\" -H \"Accept: application/vnd.github.v3+json\" --max-time 15 \"" + url + "\" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    std::string result;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    return result;
#endif
#endif
}

bool UpdateManager::httpDownload(const std::string& url, const std::string& destPath) {
    try {
        fs::create_directories(fs::path(destPath).parent_path());
    } catch (...) {}

#ifdef _WIN32
    std::wstring wurl(url.begin(), url.end());
    std::wstring host;
    std::wstring path;
    bool https = false;

    std::string lowerUrl = toLower(url);
    if (lowerUrl.rfind("https://", 0) == 0) {
        https = true;
        std::string rest = url.substr(8);
        size_t slash = rest.find('/');
        if (slash == std::string::npos) {
            host = std::wstring(rest.begin(), rest.end());
            path = L"/";
        } else {
            host = std::wstring(rest.substr(0, slash).begin(), rest.substr(0, slash).end());
            path = std::wstring(rest.substr(slash).begin(), rest.substr(slash).end());
        }
    } else if (lowerUrl.rfind("http://", 0) == 0) {
        https = false;
        std::string rest = url.substr(7);
        size_t slash = rest.find('/');
        if (slash == std::string::npos) {
            host = std::wstring(rest.begin(), rest.end());
            path = L"/";
        } else {
            host = std::wstring(rest.substr(0, slash).begin(), rest.substr(0, slash).end());
            path = std::wstring(rest.substr(slash).begin(), rest.substr(slash).end());
        }
    } else {
        return false;
    }

    HINTERNET hSession = WinHttpOpen(L"SHADOWDEEP/" L"Zv1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    INTERNET_PORT port = https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    WinHttpAddRequestHeaders(hRequest, L"User-Agent: SHADOWDEEP-Zv1\r\n", -1, WINHTTP_ADDREQ_FLAG_ADD);

    BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (bResults) bResults = WinHttpReceiveResponse(hRequest, nullptr);

    bool ok = false;
    if (bResults) {
        std::ofstream out(destPath, std::ios::binary);
        if (out) {
            DWORD dwSize = 0;
            do {
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                if (dwSize == 0) break;
                std::vector<char> buf(dwSize);
                DWORD dwDownloaded = 0;
                if (!WinHttpReadData(hRequest, buf.data(), dwSize, &dwDownloaded)) break;
                out.write(buf.data(), dwDownloaded);
            } while (dwSize > 0);
            out.close();
            ok = true;
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return ok;
#else
#ifdef SHADOWDEEP_HAS_CURL
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    std::ofstream out(destPath, std::ios::binary);
    if (!out) {
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFileCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "SHADOWDEEP-Zv1");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    out.close();
    return res == CURLE_OK;
#else
    std::string cmd = "curl -L -o \"" + destPath + "\" -A \"SHADOWDEEP-Zv1\" --max-time 60 \"" + url + "\" 2>/dev/null";
    int ret = std::system(cmd.c_str());
    return ret == 0 && fs::exists(destPath);
#endif
#endif
}

std::optional<UpdateInfo> UpdateManager::parseReleaseJson(const std::string& json, const std::string& os, const std::string& arch) {
    if (json.empty()) return std::nullopt;

    auto findStringField = [&](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return "";
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        ++pos;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;
        if (pos >= json.size() || json[pos] != '"') return "";
        ++pos;
        size_t end = pos;
        while (end < json.size()) {
            if (json[end] == '"' && json[end-1] != '\\') break;
            ++end;
        }
        if (end >= json.size()) return "";
        return json.substr(pos, end - pos);
    };

    std::string tag = findStringField("tag_name");
    if (tag.empty()) tag = findStringField("name");
    if (tag.empty()) return std::nullopt;

    std::string body = findStringField("body");

    UpdateInfo info;
    info.tag = tag;
    info.version = tag;
    info.notes = body;
    info.os = os;
    info.arch = arch;

    std::vector<std::string> candidates = getCandidateArtifactNames();

    size_t assetsPos = json.find("\"assets\"");
    if (assetsPos == std::string::npos) return std::nullopt;

    size_t arrStart = json.find('[', assetsPos);
    size_t arrEnd = json.find(']', arrStart);
    if (arrStart == std::string::npos || arrEnd == std::string::npos) return std::nullopt;

    std::string assetsBlock = json.substr(arrStart, arrEnd - arrStart + 1);

    size_t cur = 0;
    while (true) {
        size_t namePos = assetsBlock.find("\"name\"", cur);
        if (namePos == std::string::npos) break;
        size_t colon = assetsBlock.find(':', namePos);
        if (colon == std::string::npos) break;
        size_t q1 = assetsBlock.find('"', colon + 1);
        if (q1 == std::string::npos) break;
        size_t q2 = assetsBlock.find('"', q1 + 1);
        if (q2 == std::string::npos) break;
        std::string assetName = assetsBlock.substr(q1 + 1, q2 - q1 - 1);

        size_t urlPos = assetsBlock.find("\"browser_download_url\"", q2);
        if (urlPos == std::string::npos) {
            cur = q2 + 1;
            continue;
        }
        size_t colon2 = assetsBlock.find(':', urlPos);
        if (colon2 == std::string::npos) {
            cur = q2 + 1;
            continue;
        }
        size_t q3 = assetsBlock.find('"', colon2 + 1);
        if (q3 == std::string::npos) {
            cur = q2 + 1;
            continue;
        }
        size_t q4 = assetsBlock.find('"', q3 + 1);
        if (q4 == std::string::npos) {
            cur = q2 + 1;
            continue;
        }
        std::string assetUrl = assetsBlock.substr(q3 + 1, q4 - q3 - 1);

        size_t sizePos = assetsBlock.find("\"size\"", q4);
        int64_t sizeVal = 0;
        if (sizePos != std::string::npos && sizePos < assetsBlock.find("\"name\"", q4 + 1)) {
            size_t colon3 = assetsBlock.find(':', sizePos);
            if (colon3 != std::string::npos) {
                size_t numStart = colon3 + 1;
                while (numStart < assetsBlock.size() && std::isspace(static_cast<unsigned char>(assetsBlock[numStart]))) ++numStart;
                size_t numEnd = numStart;
                while (numEnd < assetsBlock.size() && std::isdigit(static_cast<unsigned char>(assetsBlock[numEnd]))) ++numEnd;
                if (numEnd > numStart) {
                    try {
                        sizeVal = std::stoll(assetsBlock.substr(numStart, numEnd - numStart));
                    } catch (...) {}
                }
            }
        }

        UpdateAsset asset;
        asset.name = assetName;
        asset.url = assetUrl;
        asset.size = sizeVal;
        info.assets.push_back(asset);

        for (auto& cand : candidates) {
            if (assetName == cand) {
                info.artifactName = assetName;
                info.downloadUrl = assetUrl;
                info.size = sizeVal;
                info.url = assetUrl;
                break;
            }
        }

        if (assetName.find("sha256") != std::string::npos || assetName.find("checksum") != std::string::npos) {
            info.checksumUrl = assetUrl;
        }

        cur = q4 + 1;
    }

    if (info.downloadUrl.empty() && !info.assets.empty()) {
        for (auto& cand : candidates) {
            std::string candLower = toLower(cand);
            for (auto& a : info.assets) {
                std::string aLower = toLower(a.name);
                if (aLower.find(toLower(os)) != std::string::npos && aLower.find(toLower(arch)) != std::string::npos) {
                    info.artifactName = a.name;
                    info.downloadUrl = a.url;
                    info.size = a.size;
                    info.url = a.url;
                    break;
                }
            }
            if (!info.downloadUrl.empty()) break;
        }
    }

    if (info.downloadUrl.empty()) {
        return std::nullopt;
    }

    info.available = true;
    return info;
}

std::optional<UpdateInfo> UpdateManager::checkForUpdate(const std::string& currentVersion) {
    std::string os = getOS();
    std::string arch = getArch();
    std::string apiUrl = getLatestReleaseApiUrl();

    std::string json = httpGet(apiUrl);
    if (json.empty()) {
        return std::nullopt;
    }

    auto infoOpt = parseReleaseJson(json, os, arch);
    if (!infoOpt) return std::nullopt;

    auto& info = *infoOpt;

    if (info.version == currentVersion) {
        if (info.version.find("Zv1") != std::string::npos) {
            return std::nullopt;
        }
    }

    return infoOpt;
}

bool UpdateManager::downloadUpdate(const UpdateInfo& info, const std::string& destPath) {
    if (info.downloadUrl.empty()) return false;
    if (info.downloadUrl.rfind("https://", 0) != 0) return false;
    return httpDownload(info.downloadUrl, destPath);
}

bool UpdateManager::verifySize(const std::string& filePath, int64_t expectedSize) {
    if (expectedSize <= 0) return true;
    try {
        auto sz = fs::file_size(filePath);
        return static_cast<int64_t>(sz) == expectedSize;
    } catch (...) {
        return false;
    }
}

static const uint32_t kSHA256[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

static void sha256Transform(uint32_t state[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<uint32_t>(block[i*4]) << 24) | (static_cast<uint32_t>(block[i*4+1]) << 16) | (static_cast<uint32_t>(block[i*4+2]) << 8) | static_cast<uint32_t>(block[i*4+3]);
    }
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = rotr(w[i-15], 7) ^ rotr(w[i-15], 18) ^ (w[i-15] >> 3);
        uint32_t s1 = rotr(w[i-2], 17) ^ rotr(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4], f = state[5], g = state[6], h = state[7];
    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t temp1 = h + S1 + ch + kSHA256[i] + w[i];
        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

std::string UpdateManager::computeSHA256(const std::string& filePath) {
    std::ifstream f(filePath, std::ios::binary);
    if (!f) return "";

    uint32_t state[8] = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };

    uint64_t totalLen = 0;
    std::vector<uint8_t> buffer;
    buffer.reserve(64);

    std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    totalLen = fileData.size();

    size_t offset = 0;
    while (offset + 64 <= fileData.size()) {
        sha256Transform(state, fileData.data() + offset);
        offset += 64;
    }

    std::vector<uint8_t> lastBlock(64, 0);
    size_t remaining = fileData.size() - offset;
    if (remaining > 0) {
        std::memcpy(lastBlock.data(), fileData.data() + offset, remaining);
    }
    lastBlock[remaining] = 0x80;
    if (remaining >= 56) {
        sha256Transform(state, lastBlock.data());
        std::memset(lastBlock.data(), 0, 64);
    }
    uint64_t bitLen = totalLen * 8;
    lastBlock[56] = static_cast<uint8_t>(bitLen >> 56);
    lastBlock[57] = static_cast<uint8_t>(bitLen >> 48);
    lastBlock[58] = static_cast<uint8_t>(bitLen >> 40);
    lastBlock[59] = static_cast<uint8_t>(bitLen >> 32);
    lastBlock[60] = static_cast<uint8_t>(bitLen >> 24);
    lastBlock[61] = static_cast<uint8_t>(bitLen >> 16);
    lastBlock[62] = static_cast<uint8_t>(bitLen >> 8);
    lastBlock[63] = static_cast<uint8_t>(bitLen);
    sha256Transform(state, lastBlock.data());

    std::ostringstream oss;
    oss << std::hex;
    for (int i = 0; i < 8; ++i) {
        oss.width(8);
        oss.fill('0');
        oss << state[i];
    }
    return oss.str();
}

bool UpdateManager::verifyChecksum(const std::string& filePath, const std::string& expectedSha256) {
    if (expectedSha256.empty()) return true;
    std::string actual = computeSHA256(filePath);
    if (actual.empty()) return false;
    std::string expLower = toLower(expectedSha256);
    std::string actLower = toLower(actual);
    return expLower == actLower;
}

}
