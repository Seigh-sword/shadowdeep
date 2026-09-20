#include "shadowdeep/save/entry_metadata.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdio>

namespace shadowdeep {

std::string EntryMetadata::statusString() const {
    switch (status) {
        case EntryStatus::Active: return "Active";
        case EntryStatus::Completed: return "Victory";
        case EntryStatus::Dead: return "Dead";
        case EntryStatus::Corrupted: return "Corrupted";
        case EntryStatus::Incompatible: return "Incompatible";
        default: return "Unknown";
    }
}

std::string EntryMetadata::lastPlayedString() const {
    if (lastPlayedTimestamp == 0) return "Never";
    std::time_t t = static_cast<std::time_t>(lastPlayedTimestamp / 1000000000);
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm);
    return std::string(buf);
}

std::string EntryMetadata::playTimeString() const {
    long long totalSec = playTimeMs / 1000;
    long long h = totalSec / 3600;
    long long m = (totalSec % 3600) / 60;
    long long s = totalSec % 60;
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%02lldh %02lldm %02llds", h, m, s);
    return std::string(buf);
}

}
