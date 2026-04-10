#ifndef GAMEOFLIFES_ENGINE_AGENT_HPP
#define GAMEOFLIFES_ENGINE_AGENT_HPP

#include "engine/direction.hpp"
#include "engine/genome.hpp"
#include "engine/memory_bank.hpp"
#include "engine/position.hpp"

#include <cstdint>

namespace gol {

static constexpr std::uint64_t NO_PARENT = UINT64_MAX;

struct Agent {
    Position pos;
    int energy;
    int hydration;
    bool alive;
    std::uint64_t id;
    Genome genome;
    std::uint64_t parent_id = NO_PARENT;
    std::uint64_t generation = 0;
    Direction facing = Direction::North;
    MemoryBank memory{};
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_AGENT_HPP
