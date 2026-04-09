#include "engine/bytecode.hpp"

#include <cstring>
#include <stdexcept>

namespace gol {

static std::uint16_t read_u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) | (static_cast<std::uint16_t>(p[1]) << 8);
}

static std::uint32_t read_u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

static std::int32_t read_i32(const std::uint8_t* p) {
    std::uint32_t u = read_u32(p);
    std::int32_t result;
    std::memcpy(&result, &u, sizeof(result));
    return result;
}

Bytecode load_bytecode(const std::vector<std::uint8_t>& blob) {
    if (blob.size() < 16) {
        throw std::runtime_error("bytecode: blob too small for header");
    }

    const std::uint8_t* p = blob.data();
    std::uint32_t magic = read_u32(p);
    if (magic != BYTECODE_MAGIC) {
        throw std::runtime_error("bytecode: invalid magic number");
    }

    std::uint16_t version = read_u16(p + 4);
    if (version != BYTECODE_VERSION) {
        throw std::runtime_error("bytecode: unsupported version");
    }

    std::uint16_t const_count = read_u16(p + 6);
    std::uint32_t code_size = read_u32(p + 8);
    std::uint16_t local_count = read_u16(p + 12);

    Bytecode bc;
    bc.version = version;
    bc.local_count = local_count;

    std::size_t offset = 16;

    // Parse constant pool.
    for (std::uint16_t i = 0; i < const_count; ++i) {
        if (offset >= blob.size()) {
            throw std::runtime_error("bytecode: unexpected end in constant pool");
        }
        std::uint8_t tag = blob[offset++];
        if (tag == 0x01) {
            // int32
            if (offset + 4 > blob.size()) {
                throw std::runtime_error("bytecode: truncated int constant");
            }
            bc.constants.push_back(read_i32(blob.data() + offset));
            offset += 4;
        } else if (tag == 0x02) {
            // string
            if (offset + 2 > blob.size()) {
                throw std::runtime_error("bytecode: truncated string length");
            }
            std::uint16_t len = read_u16(blob.data() + offset);
            offset += 2;
            if (offset + len > blob.size()) {
                throw std::runtime_error("bytecode: truncated string data");
            }
            bc.constants.push_back(
                std::string(reinterpret_cast<const char*>(blob.data() + offset), len));
            offset += len;
        } else {
            throw std::runtime_error("bytecode: unknown constant tag");
        }
    }

    // Parse code section.
    if (offset + code_size > blob.size()) {
        throw std::runtime_error("bytecode: truncated code section");
    }
    bc.code.assign(blob.data() + offset, blob.data() + offset + code_size);

    return bc;
}

Bytecode make_bytecode(const std::vector<std::uint8_t>& code, std::uint16_t local_count) {
    Bytecode bc;
    bc.local_count = local_count;
    bc.code = code;
    return bc;
}

} // namespace gol
