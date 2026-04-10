#ifndef GAMEOFLIFES_ENGINE_WORLD_HPP
#define GAMEOFLIFES_ENGINE_WORLD_HPP

#include "engine/agent.hpp"
#include "engine/grid.hpp"
#include "engine/rng.hpp"
#include "engine/terrain.hpp"
#include "engine/world_config.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace gol {

class World {
  public:
    explicit World(const WorldConfig& config);

    void init();

    Position wrap_position(Position p) const;

    std::uint64_t spawn_agent(Position pos, int energy);
    std::uint64_t spawn_agent(Position pos, int energy, const Genome& genome);
    std::uint64_t spawn_child(Position pos,
                              int energy,
                              const Genome& genome,
                              std::uint64_t parent_id,
                              std::uint64_t generation);

    const std::vector<Agent>& agents() const noexcept;
    std::vector<Agent>& agents() noexcept;

    Grid<std::uint8_t>& food() noexcept;
    const Grid<std::uint8_t>& food() const noexcept;

    Grid<std::uint8_t>& water() noexcept;
    const Grid<std::uint8_t>& water() const noexcept;

    Grid<std::uint8_t>& mineral() noexcept;
    const Grid<std::uint8_t>& mineral() const noexcept;

    Grid<Terrain>& terrain() noexcept;
    const Grid<Terrain>& terrain() const noexcept;

    const WorldConfig& config() const noexcept;
    std::uint64_t tick_count() const noexcept;
    void advance_tick() noexcept;

    Rng& rng() noexcept;

    // Rebuild per-cell agent counts and agent-id-to-index map.
    // Call once at the start of each tick before agent stepping.
    void rebuild_caches();

    // O(1) agent count at a cell (valid after rebuild_caches).
    int agent_count_at(Position p) const;

    // O(1) agent index lookup by ID (valid after rebuild_caches).
    // Returns the index or SIZE_MAX if not found.
    std::size_t agent_index_by_id(std::uint64_t id) const;

  private:
    WorldConfig config_;
    Grid<Terrain> terrain_;
    Grid<std::uint8_t> food_;
    Grid<std::uint8_t> water_;
    Grid<std::uint8_t> mineral_;
    std::vector<Agent> agents_;
    Grid<int> agent_counts_;
    std::unordered_map<std::uint64_t, std::size_t> agent_index_map_;
    Rng rng_;
    std::uint64_t tick_count_;
    std::uint64_t next_id_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_WORLD_HPP
