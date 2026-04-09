#ifndef GAMEOFLIFES_ENGINE_SNAPSHOT_HPP
#define GAMEOFLIFES_ENGINE_SNAPSHOT_HPP

#include "engine/agent.hpp"
#include "engine/genome.hpp"
#include "engine/position.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace gol {

struct SnapshotAgent {
    std::uint64_t id;
    Position pos;
    int energy;
    bool alive;
    Genome genome;
    std::uint64_t parent_id;
    std::uint64_t generation;
};

struct Snapshot {
    int version = 1;
    std::uint64_t tick;
    int width;
    int height;
    std::uint64_t seed;
    std::vector<SnapshotAgent> agents;
    // Food grid as flat row-major booleans (width * height).
    // Encoded as base64-packed bits in JSON.
    std::vector<std::uint8_t> food;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_SNAPSHOT_HPP
