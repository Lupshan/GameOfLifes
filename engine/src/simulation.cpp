#include "engine/simulation.hpp"

#include "engine/direction.hpp"
#include "engine/stats.hpp"
#include "engine/traits.hpp"

#include <algorithm>

namespace gol {

Simulation::Simulation(World& world) : world_(world) {
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

void reap_dead(World& world) {
    std::erase_if(world.agents(), [](const Agent& a) {
        return !a.alive;
    });
}

void Simulation::tick() {
    for (auto& agent : world_.agents()) {
        step_agent(agent, world_);
    }
    respawn_food(world_);
    reap_dead(world_);
    world_.advance_tick();
}

void Simulation::run(int n_ticks) {
    for (int i = 0; i < n_ticks; ++i) {
        tick();
        print_stats(world_);
    }
}

} // namespace gol
