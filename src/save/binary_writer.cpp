#include "shadowdeep/save/binary_writer.hpp"
#include <cstring>

namespace shadowdeep {

BinaryWriter::BinaryWriter() {
    buf_.reserve(4096);
}

void BinaryWriter::writeU8(uint8_t v) {
    buf_.push_back(v);
}

void BinaryWriter::writeU16(uint16_t v) {
    buf_.push_back(static_cast<uint8_t>(v & 0xFF));
    buf_.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
}

void BinaryWriter::writeU32(uint32_t v) {
    buf_.push_back(static_cast<uint8_t>(v & 0xFF));
    buf_.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    buf_.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    buf_.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
}

void BinaryWriter::writeU64(uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        buf_.push_back(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
    }
}

void BinaryWriter::writeI32(int32_t v) {
    writeU32(static_cast<uint32_t>(v));
}

void BinaryWriter::writeI64(int64_t v) {
    writeU64(static_cast<uint64_t>(v));
}

void BinaryWriter::writeBool(bool b) {
    writeU8(b ? 1 : 0);
}

void BinaryWriter::writeString(const std::string& s) {
    if (s.size() > 4096) {
        std::string truncated = s.substr(0, 4096);
        writeU32(static_cast<uint32_t>(truncated.size()));
        writeBytes(truncated.data(), truncated.size());
        return;
    }
    writeU32(static_cast<uint32_t>(s.size()));
    if (!s.empty()) writeBytes(s.data(), s.size());
}

void BinaryWriter::writeBytes(const void* data, size_t len) {
    if (len == 0) return;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    buf_.insert(buf_.end(), p, p + len);
}

void BinaryWriter::writeBytes(const std::vector<uint8_t>& data) {
    if (data.empty()) return;
    buf_.insert(buf_.end(), data.begin(), data.end());
}

void BinaryWriter::clear() {
    buf_.clear();
}

}
