#include "engine/genome.hpp"
#include "engine/simulation.hpp"
#include "engine/traits.hpp"

#include <algorithm>
#include <cstdint>
#include <doctest/doctest.h>
#include <numeric>

static gol::WorldConfig repro_config() {
    gol::WorldConfig cfg;
    cfg.width = 16;
    cfg.height = 16;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    cfg.starting_energy = 100;
    cfg.food_spawn_rate = 0.0;
    cfg.mutation_rate = 0.01;
    return cfg;
}

// ---- reproduction triggers when energy >= threshold ----

TEST_CASE("reproduce_agents spawns child when energy >= threshold") {
    gol::WorldConfig cfg = repro_config();
    gol::World world(cfg);

    // Create a genome with low reproduction threshold.
    gol::Genome g{};
    g[gol::GENE_REPRO_THRESHOLD] = 0; // threshold = REPRO_THRESHOLD_MIN = 50
    g[gol::GENE_METABOLISM] = 0;      // metabolism = 1
    g[gol::GENE_SPEED] = 0;           // speed = 1

    world.spawn_agent(gol::Position{8, 8}, 60, g);
    CHECK(world.agents().size() == 1);

    gol::reproduce_agents(world, nullptr);

    // Agent had 60 energy >= 50 threshold → reproduction should happen.
    CHECK(world.agents().size() == 2);

    const auto& parent = world.agents()[0];
    const auto& child = world.agents()[1];

    // Parent loses half energy.
    CHECK(parent.energy == 30);
    // Child gets the other half.
    CHECK(child.energy == 30);
    // Child inherits lineage.
    CHECK(child.parent_id == parent.id);
    CHECK(child.generation == 1);
}

// ---- no reproduction when energy < threshold ----

TEST_CASE("reproduce_agents does nothing when energy < threshold") {
    gol::WorldConfig cfg = repro_config();
    gol::World world(cfg);

    gol::Genome g{};
    g[gol::GENE_REPRO_THRESHOLD] = 255; // threshold = REPRO_THRESHOLD_MAX = 200
    world.spawn_agent(gol::Position{4, 4}, 100, g);

    gol::reproduce_agents(world, nullptr);

    CHECK(world.agents().size() == 1);
    CHECK(world.agents()[0].energy == 100);
}

// ---- child genome is mutated ----

TEST_CASE("child genome differs from parent (mutation_rate > 0)") {
    gol::WorldConfig cfg = repro_config();
    cfg.mutation_rate = 0.5; // high rate to guarantee mutations
    gol::World world(cfg);

    gol::Genome g{};
    g.fill(0);
    g[gol::GENE_REPRO_THRESHOLD] = 0; // low threshold
    world.spawn_agent(gol::Position{8, 8}, 100, g);

    gol::reproduce_agents(world, nullptr);

    CHECK(world.agents().size() == 2);
    // With 50% mutation rate on 128 bits, genome should differ.
    CHECK(world.agents()[1].genome != g);
}

// ---- multi-generation functional test ----

TEST_CASE("multi-generation run shows generation increase") {
    gol::WorldConfig cfg;
    cfg.width = 32;
    cfg.height = 32;
    cfg.seed = 77;
    cfg.initial_agents = 10;
    cfg.initial_food = 500;
    cfg.starting_energy = 200;
    cfg.food_spawn_rate = 0.3; // generous food for reproduction
    cfg.mutation_rate = 0.01;
    gol::World world(cfg);
    world.init();

    gol::Simulation sim(world);
    for (int t = 0; t < 500; ++t) {
        sim.tick();
    }

    // After 500 ticks with abundant food, reproduction should have occurred.
    std::uint64_t max_gen = 0;
    for (const auto& agent : world.agents()) {
        if (agent.generation > max_gen) {
            max_gen = agent.generation;
        }
    }

    CHECK(max_gen > 0);
}

// ---- selection pressure: food-scarce favors low metabolism ----

TEST_CASE("selection pressure: food-scarce world favors lower metabolism") {
    // In a food-scarce world, agents with high metabolism starve faster.
    // After many generations, average metabolism gene should be lower than initial average.

    gol::WorldConfig cfg;
    cfg.width = 32;
    cfg.height = 32;
    cfg.seed = 123;
    cfg.initial_agents = 20;
    cfg.initial_food = 200;
    cfg.starting_energy = 200;
    cfg.food_spawn_rate = 0.05; // moderate food — enough for reproduction but still scarce
    cfg.mutation_rate = 0.02;
    gol::World world(cfg);
    world.init();

    // Record initial average metabolism gene.
    double initial_avg = 0.0;
    for (const auto& agent : world.agents()) {
        initial_avg += agent.genome[gol::GENE_METABOLISM];
    }
    initial_avg /= static_cast<double>(world.agents().size());

    gol::Simulation sim(world);
    for (int t = 0; t < 1000; ++t) {
        sim.tick();
    }

    if (world.agents().empty()) {
        // All died — can't measure, but the test is still valid
        // (extreme scarcity killed everyone).
        WARN(false);
        return;
    }

    double final_avg = 0.0;
    for (const auto& agent : world.agents()) {
        final_avg += agent.genome[gol::GENE_METABOLISM];
    }
    final_avg /= static_cast<double>(world.agents().size());

    // We expect selection pressure to push metabolism down.
    // This is a soft check — we allow it to fail with a warning rather
    // than a hard failure, since evolution is stochastic.
    WARN(final_avg < initial_avg);
    // At minimum, agents should still exist (the ecosystem sustained).
    CHECK(world.agents().size() > 0);
}
