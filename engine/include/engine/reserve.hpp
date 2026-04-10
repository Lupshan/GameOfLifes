#ifndef GAMEOFLIFES_ENGINE_RESERVE_HPP
#define GAMEOFLIFES_ENGINE_RESERVE_HPP

#include "engine/position.hpp"

#include <string>

namespace gol {

// A Reserve is a sub-region of the world dedicated to Tier 3 live-learning bots.
// It has its own CPU budget and isolation boundaries.
struct Reserve {
    std::string id;
    Position origin; // Top-left corner.
    int width;
    int height;
    int max_agents;    // Max agents allowed in this reserve.
    int cpu_budget_ms; // Max ms of computation per tick for this reserve.
};

// Check if a position is inside a reserve.
bool inside_reserve(const Reserve& r, Position p);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_RESERVE_HPP
