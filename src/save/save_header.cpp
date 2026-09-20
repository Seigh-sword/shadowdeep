#include "shadowdeep/save/save_header.hpp"
#include "shadowdeep/version.hpp"
#include <chrono>
#include <random>

namespace shadowdeep {

bool SaveHeader::valid() const {
    return magic == kSaveMagic && containerVersion >= 1 && saveSchema >= 1;
}

SaveHeader SaveHeader::makeNew(uint16_t schemaVersion) {
    SaveHeader h;
    h.magic = kSaveMagic;
    h.containerVersion = kContainerFormatVersion;
    h.saveSchema = schemaVersion;
    h.creationTimestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
    h.lastSaveTimestamp = h.creationTimestamp;
    h.payloadSize = 0;
    h.compression = static_cast<uint8_t>(CompressionId::None);
    h.headerCrc = 0;
    h.payloadCrc = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 255);
    for (int i = 0; i < 16; ++i) h.entryUuid[i] = static_cast<uint8_t>(dis(gen));

    return h;
}

}
