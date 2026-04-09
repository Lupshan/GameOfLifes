#ifndef GAMEOFLIFES_ENGINE_BYTECODE_HPP
#define GAMEOFLIFES_ENGINE_BYTECODE_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace gol {

// Opcode constants (see bytecode/spec/opcodes.md).
namespace op {
static constexpr std::uint8_t HALT = 0x00;

static constexpr std::uint8_t PUSH_INT = 0x01;
static constexpr std::uint8_t PUSH_BOOL = 0x02;
static constexpr std::uint8_t POP = 0x03;
static constexpr std::uint8_t DUP = 0x04;
static constexpr std::uint8_t SWAP = 0x05;

static constexpr std::uint8_t ADD = 0x10;
static constexpr std::uint8_t SUB = 0x11;
static constexpr std::uint8_t MUL = 0x12;
static constexpr std::uint8_t DIV = 0x13;
static constexpr std::uint8_t MOD = 0x14;
static constexpr std::uint8_t NEG = 0x15;

static constexpr std::uint8_t EQ = 0x20;
static constexpr std::uint8_t LT = 0x21;
static constexpr std::uint8_t GT = 0x22;
static constexpr std::uint8_t AND = 0x23;
static constexpr std::uint8_t OR = 0x24;
static constexpr std::uint8_t NOT = 0x25;

static constexpr std::uint8_t JMP = 0x30;
static constexpr std::uint8_t JMP_IF_FALSE = 0x31;
static constexpr std::uint8_t CALL = 0x32;
static constexpr std::uint8_t RETURN = 0x33;

static constexpr std::uint8_t LOAD_LOCAL = 0x40;
static constexpr std::uint8_t STORE_LOCAL = 0x41;

static constexpr std::uint8_t LOAD_CONST = 0x50;

// Agent intrinsics (0x80-0x8F) — implemented in step 14.
static constexpr std::uint8_t PERCEIVE = 0x80;
static constexpr std::uint8_t LOOK = 0x81;
static constexpr std::uint8_t MOVE = 0x82;
static constexpr std::uint8_t TURN_LEFT = 0x83;
static constexpr std::uint8_t TURN_RIGHT = 0x84;
static constexpr std::uint8_t EAT = 0x85;
static constexpr std::uint8_t DRINK = 0x86;
static constexpr std::uint8_t REPRODUCE = 0x87;
static constexpr std::uint8_t MY_ENERGY = 0x88;
static constexpr std::uint8_t MY_HYDRATION = 0x89;
static constexpr std::uint8_t MY_TRAIT = 0x8A;

static constexpr std::uint8_t NOP = 0xFF;
} // namespace op

// Magic number: "GOLB"
static constexpr std::uint32_t BYTECODE_MAGIC = 0x474F4C42;
static constexpr std::uint16_t BYTECODE_VERSION = 1;

using Constant = std::variant<std::int32_t, std::string>;

struct Bytecode {
    std::uint16_t version = BYTECODE_VERSION;
    std::uint16_t local_count = 0;
    std::vector<Constant> constants;
    std::vector<std::uint8_t> code;
};

// Load bytecode from a raw binary blob.
// Returns empty optional on parse error.
Bytecode load_bytecode(const std::vector<std::uint8_t>& blob);

// Build a bytecode from raw code bytes (for testing convenience).
Bytecode make_bytecode(const std::vector<std::uint8_t>& code, std::uint16_t local_count = 0);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_BYTECODE_HPP
