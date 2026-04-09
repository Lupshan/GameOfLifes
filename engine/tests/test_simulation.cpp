#include "engine/genome.hpp"
#include "engine/simulation.hpp"
#include "engine/traits.hpp"

#include <doctest/doctest.h>

static gol::WorldConfig tiny_config() {
    gol::WorldConfig cfg;
    cfg.width = 8;
    cfg.height = 8;
    cfg.seed = 42;
    cfg.initial_agents = 4;
    cfg.initial_food = 20;
    cfg.starting_energy = 50;
    cfg.food_spawn_rate = 0.05;
    cfg.max_ticks = 0;
    return cfg;
}

// ---- unit: step_agent eats food ----

TEST_CASE("step_agent consumes food and gains energy") {
    gol::WorldConfig cfg = tiny_config();
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    gol::World world(cfg);

    // Use a genome with speed=1, metabolism=1 for predictable behavior.
    gol::Genome g{};
    g[gol::GENE_SPEED] = 0;      // speed = 1
    g[gol::GENE_METABOLISM] = 0; // metabolism = 1

    // place agent at center, fill all neighbours with food
    world.spawn_agent(gol::Position{4, 4}, 10, g);
    world.food().set(gol::Position{4, 3}, 1);
    world.food().set(gol::Position{4, 5}, 1);
    world.food().set(gol::Position{3, 4}, 1);
    world.food().set(gol::Position{5, 4}, 1);

    gol::Agent& agent = world.agents()[0];
    gol::step_agent(agent, world);

    // agent moved to one neighbour, ate the food there
    // energy: started 10, +1 food, -1 metabolism = 10
    CHECK(agent.energy == 10);
    CHECK(world.food().at(agent.pos) == 0);
}

// ---- unit: step_agent energy drain ----

TEST_CASE("step_agent drains energy and kills at zero") {
    gol::WorldConfig cfg = tiny_config();
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    gol::World world(cfg);

    gol::Genome g{};
    g[gol::GENE_SPEED] = 0;      // speed = 1
    g[gol::GENE_METABOLISM] = 0; // metabolism = 1
    world.spawn_agent(gol::Position{0, 0}, 1, g);

    gol::Agent& agent = world.agents()[0];
    gol::step_agent(agent, world);

    // 1 energy - 1 metabolism = 0 → dead
    CHECK(agent.energy <= 0);
    CHECK_FALSE(agent.alive);
}

// ---- unit: reap_dead removes dead agents ----

TEST_CASE("reap_dead removes dead agents") {
    gol::WorldConfig cfg = tiny_config();
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    gol::World world(cfg);

    world.spawn_agent(gol::Position{0, 0}, 10);
    world.spawn_agent(gol::Position{1, 1}, 10);
    world.agents()[1].alive = false;

    gol::reap_dead(world, nullptr);

    CHECK(world.agents().size() == 1);
    CHECK(world.agents()[0].id == 0);
}

// ---- functional: 100-tick invariants ----

TEST_CASE("100-tick run: positions in bounds and alive implies energy > 0") {
    gol::WorldConfig cfg = tiny_config();
    gol::World world(cfg);
    world.init();
    gol::Simulation sim(world);

    for (int t = 0; t < 100; ++t) {
        sim.tick();

        for (const auto& agent : world.agents()) {
            CHECK(agent.pos.x >= 0);
            CHECK(agent.pos.x < cfg.width);
            CHECK(agent.pos.y >= 0);
            CHECK(agent.pos.y < cfg.height);
            if (agent.alive) {
                CHECK(agent.energy > 0);
            }
        }
    }
}

// ---- functional: starvation bounded ----

TEST_CASE("starvation: no food means all agents die within starting_energy ticks") {
    gol::WorldConfig cfg = tiny_config();
    cfg.initial_food = 0;
    cfg.food_spawn_rate = 0.0;
    cfg.starting_energy = 20;
    cfg.initial_agents = 8;
    gol::World world(cfg);
    world.init();
    gol::Simulation sim(world);

    // with no food and no respawn, agents must all die within starting_energy ticks
    for (int t = 0; t < cfg.starting_energy; ++t) {
        sim.tick();
    }

    CHECK(world.agents().empty());
}
