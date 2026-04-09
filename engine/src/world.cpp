#include "engine/world.hpp"

namespace gol {

World::World(const WorldConfig& config)
    : config_(config), food_(config.width, config.height, 0), rng_(config.seed), tick_count_(0),
      next_id_(0) {
}

void World::init() {
    for (int i = 0; i < config_.initial_food; ++i) {
        int x = rng_.uniform_int(0, config_.width - 1);
        int y = rng_.uniform_int(0, config_.height - 1);
        food_.set(Position{x, y}, 1);
    }
    for (int i = 0; i < config_.initial_agents; ++i) {
        int x = rng_.uniform_int(0, config_.width - 1);
        int y = rng_.uniform_int(0, config_.height - 1);
        spawn_agent(Position{x, y}, config_.starting_energy);
    }
}

Position World::wrap_position(Position p) const {
    return Position{wrap(p.x, config_.width), wrap(p.y, config_.height)};
}

std::uint64_t World::spawn_agent(Position pos, int energy) {
    Genome g = random_genome(rng_);
    return spawn_agent(pos, energy, g);
}

std::uint64_t World::spawn_agent(Position pos, int energy, const Genome& genome) {
    std::uint64_t id = next_id_++;
    agents_.push_back(Agent{pos, energy, true, id, genome});
    return id;
}

const std::vector<Agent>& World::agents() const noexcept {
    return agents_;
}

std::vector<Agent>& World::agents() noexcept {
    return agents_;
}

Grid<std::uint8_t>& World::food() noexcept {
    return food_;
}

const Grid<std::uint8_t>& World::food() const noexcept {
    return food_;
}

const WorldConfig& World::config() const noexcept {
    return config_;
}

std::uint64_t World::tick_count() const noexcept {
    return tick_count_;
}

void World::advance_tick() noexcept {
    ++tick_count_;
}

Rng& World::rng() noexcept {
    return rng_;
}

} // namespace gol
