#include "engine/simulation.hpp"

#include "engine/direction.hpp"
#include "engine/genome.hpp"
#include "engine/stats.hpp"
#include "engine/traits.hpp"

#include <algorithm>
#include <cstddef>

namespace gol {

Simulation::Simulation(World& world) : world_(world) {
}

Simulation::Simulation(World& world, LineageLog& log) : world_(world), log_(&log) {
}

void step_agent(Agent& agent, World& world) {
    if (!agent.alive) {
        return;
    }

    Traits traits = decode(agent.genome);

    // Speed determines how many moves the agent gets per tick.
    for (int step = 0; step < traits.speed; ++step) {
        int dir_index = world.rng().uniform_int(0, 3);
        auto dir = static_cast<Direction>(dir_index);
        Position offset = to_offset(dir);

        Position raw{agent.pos.x + offset.x, agent.pos.y + offset.y};
        agent.pos = world.wrap_position(raw);

        std::uint8_t& cell = world.food().at(agent.pos);
        if (cell > 0) {
            agent.energy += cell;
            cell = 0;
        }
    }

    // Metabolism determines energy drain per tick.
    agent.energy -= traits.metabolism;
    if (agent.energy <= 0) {
        agent.alive = false;
    }
}

void reproduce_agents(World& world, LineageLog* log) {
    // Collect births to avoid mutating the vector while iterating.
    struct Birth {
        Position pos;
        int energy;
        Genome genome;
        std::uint64_t parent_id;
        std::uint64_t generation;
    };
    std::vector<Birth> births;

    for (auto& agent : world.agents()) {
        if (!agent.alive) {
            continue;
        }

        Traits traits = decode(agent.genome);
        if (agent.energy < traits.reproduction_threshold) {
            continue;
        }

        // Find a random adjacent cell to place the child.
        int dir_index = world.rng().uniform_int(0, 3);
        auto dir = static_cast<Direction>(dir_index);
        Position offset = to_offset(dir);
        Position child_pos =
            world.wrap_position(Position{agent.pos.x + offset.x, agent.pos.y + offset.y});

        // Parent loses half energy, child gets the other half.
        int child_energy = agent.energy / 2;
        agent.energy -= child_energy;

        Genome child_genome = mutate(agent.genome, world.rng(), world.config().mutation_rate);

        births.push_back(
            Birth{child_pos, child_energy, child_genome, agent.id, agent.generation + 1});
    }

    for (const auto& b : births) {
        std::uint64_t child_id =
            world.spawn_child(b.pos, b.energy, b.genome, b.parent_id, b.generation);
        if (log != nullptr) {
            log->log_birth(world.tick_count(), child_id, b.parent_id, b.generation, b.genome);
        }
    }
}

void respawn_food(World& world) {
    double rate = world.config().food_spawn_rate;
    int w = world.config().width;
    int h = world.config().height;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Position p{x, y};
            if (world.food().at(p) == 0 && world.rng().chance(rate)) {
                world.food().set(p, 1);
            }
        }
    }
}

void reap_dead(World& world, LineageLog* log) {
    if (log != nullptr) {
        for (const auto& agent : world.agents()) {
            if (!agent.alive) {
                log->log_death(world.tick_count(), agent.id, "starvation");
            }
        }
    }
    std::erase_if(world.agents(), [](const Agent& a) {
        return !a.alive;
    });
}

void Simulation::tick() {
    for (auto& agent : world_.agents()) {
        step_agent(agent, world_);
    }
    reproduce_agents(world_, log_);
    respawn_food(world_);
    reap_dead(world_, log_);
    world_.advance_tick();
}

void Simulation::run(int n_ticks) {
    for (int i = 0; i < n_ticks; ++i) {
        tick();
        print_stats(world_);
    }
}

} // namespace gol
