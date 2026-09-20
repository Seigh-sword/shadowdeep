#include "shadowdeep/save/binary_writer.hpp"
#include "shadowdeep/save/binary_reader.hpp"
#include <cassert>

void test_save_roundtrip() {
    shadowdeep::BinaryWriter w;
    w.writeU32(0x12345678);
    w.writeString("hello");
    w.writeBool(true);
    w.writeI32(-42);

    auto data = w.data();
    shadowdeep::BinaryReader r(data);

    uint32_t u = 0;
    assert(r.readU32(u));
    assert(u == 0x12345678);

    std::string s;
    assert(r.readString(s));
    assert(s == "hello");

    bool b = false;
    assert(r.readBool(b));
    assert(b == true);

    int32_t i = 0;
    assert(r.readI32(i));
    assert(i == -42);
}
