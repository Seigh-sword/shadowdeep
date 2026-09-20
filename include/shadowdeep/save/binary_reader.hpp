#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace shadowdeep {

class BinaryReader {
public:
    explicit BinaryReader(const std::vector<uint8_t>& data);
    explicit BinaryReader(const uint8_t* data, size_t len);

    bool eof() const;
    size_t remaining() const;
    size_t position() const;

    bool readU8(uint8_t& out);
    bool readU16(uint16_t& out);
    bool readU32(uint32_t& out);
    bool readU64(uint64_t& out);
    bool readI32(int32_t& out);
    bool readI64(int64_t& out);
    bool readBool(bool& out);
    bool readString(std::string& out);
    bool readBytes(size_t len, std::vector<uint8_t>& out);
    bool readBytes(size_t len, uint8_t* out);

    bool checkMagic(uint32_t expected);

private:
    const uint8_t* data_ = nullptr;
    size_t len_ = 0;
    size_t pos_ = 0;
    std::vector<uint8_t> owned_;
    bool isOwned_ = false;
};

}
