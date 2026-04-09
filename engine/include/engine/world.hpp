#ifndef GAMEOFLIFES_ENGINE_WORLD_HPP
#define GAMEOFLIFES_ENGINE_WORLD_HPP

#include "engine/agent.hpp"
#include "engine/grid.hpp"
#include "engine/rng.hpp"
#include "engine/world_config.hpp"

#include <cstdint>
#include <vector>

namespace gol {

class World {
  public:
    explicit World(const WorldConfig& config);

    void init();

    Position wrap_position(Position p) const;

    std::uint64_t spawn_agent(Position pos, int energy);

    const std::vector<Agent>& agents() const noexcept;
    std::vector<Agent>& agents() noexcept;

    Grid<std::uint8_t>& food() noexcept;
    const Grid<std::uint8_t>& food() const noexcept;

    const WorldConfig& config() const noexcept;
    std::uint64_t tick_count() const noexcept;
    void advance_tick() noexcept;

    Rng& rng() noexcept;

  private:
    WorldConfig config_;
    Grid<std::uint8_t> food_;
    std::vector<Agent> agents_;
    Rng rng_;
    std::uint64_t tick_count_;
    std::uint64_t next_id_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_WORLD_HPP
