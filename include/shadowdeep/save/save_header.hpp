#pragma once
#include <cstdint>
#include <string>
#include <array>

namespace shadowdeep {

constexpr uint32_t kSaveMagic = 0x53484450;
constexpr uint32_t kSaveMagicSwapped = 0x50444853;

struct SaveHeader {
    uint32_t magic = kSaveMagic;
    uint16_t containerVersion = 1;
    uint16_t saveSchema = 1;
    std::array<uint8_t, 16> entryUuid{};
    uint64_t creationTimestamp = 0;
    uint64_t lastSaveTimestamp = 0;
    uint64_t payloadSize = 0;
    uint8_t compression = 0;
    uint8_t reserved[3]{};
    uint32_t headerCrc = 0;
    uint32_t payloadCrc = 0;

    bool valid() const;
    static SaveHeader makeNew(uint16_t schemaVersion);
};

enum class CompressionId : uint8_t {
    None = 0,
    Zstd = 1
};

}
