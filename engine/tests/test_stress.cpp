#include "engine/simulation.hpp"
#include "engine/traits.hpp"

#include <doctest/doctest.h>

static gol::WorldConfig stress_config() {
    gol::WorldConfig cfg;
    cfg.width = 64;
    cfg.height = 64;
    cfg.seed = 1234;
    cfg.initial_agents = 1000;
    cfg.initial_food = 2000;
    cfg.starting_energy = 200;
    cfg.starting_hydration = 200;
    cfg.food_spawn_rate = 0.1;
    cfg.mutation_rate = 0.02;
    cfg.max_ticks = 0;
    return cfg;
}

TEST_CASE("stress: 1000 agents survive 100 ticks without crash") {
    gol::WorldConfig cfg = stress_config();
    gol::World world(cfg);
    world.init();

    CHECK(world.agents().size() == 1000);

    gol::Simulation sim(world);
    for (int i = 0; i < 100; ++i) {
        sim.tick();
    }

    // After 100 ticks the world is still consistent.
    CHECK(world.tick_count() == 100);

    // Some agents should still be alive (food + energy are generous).
    std::size_t alive = 0;
    for (const auto& a : world.agents()) {
        if (a.alive) {
            ++alive;
        }
    }
    CHECK(alive > 0);
}

TEST_CASE("stress: no ID collision after many births") {
    gol::WorldConfig cfg = stress_config();
    cfg.starting_energy = 500;
    cfg.starting_hydration = 500;
    cfg.initial_agents = 200;
    gol::World world(cfg);
    world.init();

    gol::Simulation sim(world);
    for (int i = 0; i < 50; ++i) {
        sim.tick();
    }

    // Verify all IDs are unique.
    std::unordered_set<std::uint64_t> ids;
    for (const auto& a : world.agents()) {
        CHECK(ids.insert(a.id).second);
    }
}

TEST_CASE("stress: agent_count cache matches brute-force count") {
    gol::WorldConfig cfg;
    cfg.width = 16;
    cfg.height = 16;
    cfg.seed = 99;
    cfg.initial_agents = 100;
    cfg.initial_food = 50;
    cfg.starting_energy = 200;
    cfg.starting_hydration = 200;
    gol::World world(cfg);
    world.init();
    world.rebuild_caches();

    // Compare cached agent counts against brute-force.
    for (int y = 0; y < cfg.height; ++y) {
        for (int x = 0; x < cfg.width; ++x) {
            gol::Position p{x, y};
            int expected = 0;
            for (const auto& a : world.agents()) {
                if (a.alive && a.pos == p) {
                    ++expected;
                }
            }
            CHECK(world.agent_count_at(p) == expected);
        }
    }
}
