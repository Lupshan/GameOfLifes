#ifndef GAMEOFLIFES_ENGINE_AGENT_HPP
#define GAMEOFLIFES_ENGINE_AGENT_HPP

#include "engine/position.hpp"

#include <cstdint>

namespace gol {

struct Agent {
    Position pos;
    int energy;
    bool alive;
    std::uint64_t id;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_AGENT_HPP
