#pragma once
#include <cstdint>
#include <string>
#include <chrono>

namespace shadowdeep {

enum class EntryStatus : uint8_t {
    Active = 0,
    Completed = 1,
    Dead = 2,
    Corrupted = 3,
    Incompatible = 4
};

struct EntryMetadata {
    std::string entryId;
    std::string entryName;
    std::string characterName;
    std::string className;
    int level = 1;
    int depth = 1;
    std::string regionName;
    long long playTimeMs = 0;
    int64_t lastPlayedTimestamp = 0;
    int64_t creationTimestamp = 0;
    std::string seed;
    std::string difficulty;
    EntryStatus status = EntryStatus::Active;
    uint16_t saveSchema = 1;
    std::string gameVersion;
    int gold = 0;
    int rubies = 0;
    int kills = 0;
    bool hasAmulet = false;
    size_t fileSize = 0;
    bool valid = false;

    std::string statusString() const;
    std::string lastPlayedString() const;
    std::string playTimeString() const;
};

}
