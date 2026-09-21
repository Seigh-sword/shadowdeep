#include "shadowdeep/save/binary_reader.hpp"
#include <cstring>

namespace shadowdeep {

BinaryReader::BinaryReader(const std::vector<uint8_t>& data) : data_(data.data()), len_(data.size()), pos_(0), owned_(data), isOwned_(true) {
    data_ = owned_.data();
}

BinaryReader::BinaryReader(const uint8_t* data, size_t len) : data_(data), len_(len), pos_(0), isOwned_(false) {}

bool BinaryReader::eof() const {
    return pos_ >= len_;
}

size_t BinaryReader::remaining() const {
    return pos_ < len_ ? len_ - pos_ : 0;
}

size_t BinaryReader::position() const {
    return pos_;
}

bool BinaryReader::readU8(uint8_t& out) {
    if (pos_ + 1 > len_) return false;
    out = data_[pos_];
    pos_ += 1;
    return true;
}

bool BinaryReader::readU16(uint16_t& out) {
    if (pos_ + 2 > len_) return false;
    out = static_cast<uint16_t>(static_cast<uint16_t>(data_[pos_]) | (static_cast<uint16_t>(data_[pos_ + 1]) << 8));
    pos_ += 2;
    return true;
}

bool BinaryReader::readU32(uint32_t& out) {
    if (pos_ + 4 > len_) return false;
    out = static_cast<uint32_t>(data_[pos_]) |
          (static_cast<uint32_t>(data_[pos_ + 1]) << 8) |
          (static_cast<uint32_t>(data_[pos_ + 2]) << 16) |
          (static_cast<uint32_t>(data_[pos_ + 3]) << 24);
    pos_ += 4;
    return true;
}

bool BinaryReader::readU64(uint64_t& out) {
    if (pos_ + 8 > len_) return false;
    out = 0;
    for (int i = 0; i < 8; ++i) {
        out |= static_cast<uint64_t>(data_[pos_ + i]) << static_cast<uint64_t>(i * 8);
    }
    pos_ += 8;
    return true;
}

bool BinaryReader::readI32(int32_t& out) {
    uint32_t u = 0;
    if (!readU32(u)) return false;
    out = static_cast<int32_t>(u);
    return true;
}

bool BinaryReader::readI64(int64_t& out) {
    uint64_t u = 0;
    if (!readU64(u)) return false;
    out = static_cast<int64_t>(u);
    return true;
}

bool BinaryReader::readBool(bool& out) {
    uint8_t b = 0;
    if (!readU8(b)) return false;
    out = b != 0;
    return true;
}

bool BinaryReader::readString(std::string& out) {
    uint32_t len = 0;
    if (!readU32(len)) return false;
    if (len > 4096) return false;
    if (pos_ + len > len_) return false;
    out.assign(reinterpret_cast<const char*>(data_ + pos_), len);
    pos_ += len;
    return true;
}

bool BinaryReader::readBytes(size_t len, std::vector<uint8_t>& out) {
    if (pos_ + len > len_) return false;
    out.assign(data_ + pos_, data_ + pos_ + len);
    pos_ += len;
    return true;
}

bool BinaryReader::readBytes(size_t len, uint8_t* out) {
    if (pos_ + len > len_) return false;
    std::memcpy(out, data_ + pos_, len);
    pos_ += len;
    return true;
}

bool BinaryReader::checkMagic(uint32_t expected) {
    uint32_t v = 0;
    size_t saved = pos_;
    if (!readU32(v)) return false;
    if (v != expected) {
        pos_ = saved;
        return false;
    }
    return true;
}

}
