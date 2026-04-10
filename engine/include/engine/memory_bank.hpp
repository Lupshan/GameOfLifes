#ifndef GAMEOFLIFES_ENGINE_MEMORY_BANK_HPP
#define GAMEOFLIFES_ENGINE_MEMORY_BANK_HPP

#include <array>
#include <cstdint>

namespace gol {

static constexpr int MEMORY_BANK_SIZE = 16;

// Per-agent memory bank: 16 ints that persist across ticks.
using MemoryBank = std::array<std::int32_t, MEMORY_BANK_SIZE>;

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_MEMORY_BANK_HPP
