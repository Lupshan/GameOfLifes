#include "engine/world.hpp"

namespace gol {

World::World(const WorldConfig& config)
    : config_(config), terrain_(config.width, config.height, Terrain::Plain),
      food_(config.width, config.height, 0), water_(config.width, config.height, 0),
      mineral_(config.width, config.height, 0), agent_counts_(config.width, config.height, 0),
      rng_(config.seed), tick_count_(0), next_id_(0) {
}

void World::init() {
    // Generate terrain: simple noise-like distribution based on RNG.
    for (int y = 0; y < config_.height; ++y) {
        for (int x = 0; x < config_.width; ++x) {
            int roll = rng_.uniform_int(0, 99);
            Terrain t = Terrain::Plain;
            if (roll < 30) {
                t = Terrain::Forest;
            } else if (roll < 40) {
                t = Terrain::Water;
            } else if (roll < 50) {
                t = Terrain::Rock;
            }
            terrain_.set(Position{x, y}, t);
        }
    }

    // Place initial food.
    for (int i = 0; i < config_.initial_food; ++i) {
        int x = rng_.uniform_int(0, config_.width - 1);
        int y = rng_.uniform_int(0, config_.height - 1);
        food_.set(Position{x, y}, 1);
    }

    // Place some initial water on water terrain.
    for (int y = 0; y < config_.height; ++y) {
        for (int x = 0; x < config_.width; ++x) {
            Position p{x, y};
            if (terrain_.at(p) == Terrain::Water) {
                water_.set(p, 1);
            }
        }
    }

    // Place some initial minerals on rock terrain.
    for (int y = 0; y < config_.height; ++y) {
        for (int x = 0; x < config_.width; ++x) {
            Position p{x, y};
            if (terrain_.at(p) == Terrain::Rock && rng_.chance(0.3)) {
                mineral_.set(p, 1);
            }
        }
    }

    // Spawn initial agents.
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
    Agent a;
    a.pos = pos;
    a.energy = energy;
    a.alive = true;
    a.id = id;
    a.genome = genome;
    a.parent_id = NO_PARENT;
    a.generation = 0;
    a.hydration = config_.starting_hydration;
    agents_.push_back(a);
    return id;
}

std::uint64_t World::spawn_child(Position pos,
                                 int energy,
                                 const Genome& genome,
                                 std::uint64_t parent_id,
                                 std::uint64_t generation) {
    std::uint64_t id = next_id_++;
    Agent a;
    a.pos = pos;
    a.energy = energy;
    a.alive = true;
    a.id = id;
    a.genome = genome;
    a.parent_id = parent_id;
    a.generation = generation;
    a.hydration = energy; // child inherits half parent's hydration proportional to energy
    agents_.push_back(a);
    return id;
}

void World::rebuild_caches() {
    // Zero out agent count grid.
    for (int y = 0; y < config_.height; ++y) {
        for (int x = 0; x < config_.width; ++x) {
            agent_counts_.set(Position{x, y}, 0);
        }
    }

    agent_index_map_.clear();
    agent_index_map_.reserve(agents_.size());

    for (std::size_t i = 0; i < agents_.size(); ++i) {
        const auto& a = agents_[i];
        agent_index_map_[a.id] = i;
        if (a.alive) {
            agent_counts_.at(a.pos) += 1;
        }
    }
}

int World::agent_count_at(Position p) const {
    return agent_counts_.at(p);
}

std::size_t World::agent_index_by_id(std::uint64_t id) const {
    auto it = agent_index_map_.find(id);
    if (it != agent_index_map_.end()) {
        return it->second;
    }
    return SIZE_MAX;
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

Grid<std::uint8_t>& World::water() noexcept {
    return water_;
}

const Grid<std::uint8_t>& World::water() const noexcept {
    return water_;
}

Grid<std::uint8_t>& World::mineral() noexcept {
    return mineral_;
}

const Grid<std::uint8_t>& World::mineral() const noexcept {
    return mineral_;
}

Grid<Terrain>& World::terrain() noexcept {
    return terrain_;
}

const Grid<Terrain>& World::terrain() const noexcept {
    return terrain_;
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
