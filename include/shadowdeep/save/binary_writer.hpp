#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace shadowdeep {

class BinaryWriter {
public:
    BinaryWriter();

    void writeU8(uint8_t v);
    void writeU16(uint16_t v);
    void writeU32(uint32_t v);
    void writeU64(uint64_t v);
    void writeI32(int32_t v);
    void writeI64(int64_t v);
    void writeBool(bool b);
    void writeString(const std::string& s);
    void writeBytes(const void* data, size_t len);
    void writeBytes(const std::vector<uint8_t>& data);

    const std::vector<uint8_t>& data() const {
        return buf_;
    }

    std::vector<uint8_t> take() {
        return std::move(buf_);
    }

    size_t size() const {
        return buf_.size();
    }

    void clear();

private:
    std::vector<uint8_t> buf_;
};

}
