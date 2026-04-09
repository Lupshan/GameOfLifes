#include "engine/simulation.hpp"

#include "engine/direction.hpp"
#include "engine/genome.hpp"
#include "engine/stats.hpp"
#include "engine/terrain.hpp"
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

        Position target =
            world.wrap_position(Position{agent.pos.x + offset.x, agent.pos.y + offset.y});

        // Terrain movement cost. Swim trait reduces water cost.
        Terrain t = world.terrain().at(target);
        int cost = movement_cost(t);
        if (t == Terrain::Water && traits.swim > 0) {
            cost = std::max(1, cost - traits.swim);
        }

        // Agent pays extra energy for difficult terrain.
        agent.energy -= (cost - 1);
        agent.pos = target;

        // Forage: pick up food.
        std::uint8_t& food_cell = world.food().at(agent.pos);
        if (food_cell > 0) {
            int gain = food_cell * traits.forage_efficiency;
            agent.energy += gain;
            food_cell = 0;
        }

        // Drink water.
        std::uint8_t& water_cell = world.water().at(agent.pos);
        if (water_cell > 0) {
            int gain = water_cell * traits.forage_efficiency;
            agent.hydration += gain;
            water_cell = 0;
        }

        // Collect mineral (stored as energy bonus for now).
        std::uint8_t& mineral_cell = world.mineral().at(agent.pos);
        if (mineral_cell > 0) {
            agent.energy += mineral_cell;
            mineral_cell = 0;
        }
    }

    // Metabolism determines energy drain per tick.
    agent.energy -= traits.metabolism;

    // Hydration drain: 1 per tick.
    agent.hydration -= 1;

    // Die from starvation or dehydration.
    if (agent.energy <= 0 || agent.hydration <= 0) {
        agent.alive = false;
    }
}

void reproduce_agents(World& world, LineageLog* log) {
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

        // Reproduction requires energy >= threshold AND mineral availability.
        // Check if any adjacent cell has a mineral, or agent has enough energy
        // to reproduce without mineral (threshold * 1.5).
        if (agent.energy < traits.reproduction_threshold) {
            continue;
        }

        int dir_index = world.rng().uniform_int(0, 3);
        auto dir = static_cast<Direction>(dir_index);
        Position offset = to_offset(dir);
        Position child_pos =
            world.wrap_position(Position{agent.pos.x + offset.x, agent.pos.y + offset.y});

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

void respawn_resources(World& world) {
    double base_food_rate = world.config().food_spawn_rate;
    double base_water_rate = base_food_rate * 0.5;
    double base_mineral_rate = base_food_rate * 0.1;
    int w = world.config().width;
    int h = world.config().height;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Position p{x, y};
            Terrain t = world.terrain().at(p);

            if (world.food().at(p) == 0) {
                double rate = base_food_rate * food_regen_factor(t);
                if (world.rng().chance(rate)) {
                    world.food().set(p, 1);
                }
            }

            if (world.water().at(p) == 0) {
                double rate = base_water_rate * water_regen_factor(t);
                if (world.rng().chance(rate)) {
                    world.water().set(p, 1);
                }
            }

            if (world.mineral().at(p) == 0) {
                double rate = base_mineral_rate * mineral_regen_factor(t);
                if (world.rng().chance(rate)) {
                    world.mineral().set(p, 1);
                }
            }
        }
    }
}

void reap_dead(World& world, LineageLog* log) {
    if (log != nullptr) {
        for (const auto& agent : world.agents()) {
            if (!agent.alive) {
                std::string cause = (agent.hydration <= 0) ? "dehydration" : "starvation";
                log->log_death(world.tick_count(), agent.id, cause);
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
    respawn_resources(world_);
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
