#include "engine/simulation.hpp"
#include "engine/world.hpp"
#include "engine/world_clock.hpp"

#include <doctest/doctest.h>

// ---- day_phase ----

TEST_CASE("day_phase wraps correctly") {
    CHECK(gol::day_phase(0, 100) == doctest::Approx(0.0));
    CHECK(gol::day_phase(25, 100) == doctest::Approx(0.25));
    CHECK(gol::day_phase(50, 100) == doctest::Approx(0.5));
    CHECK(gol::day_phase(75, 100) == doctest::Approx(0.75));
    CHECK(gol::day_phase(100, 100) == doctest::Approx(0.0)); // wraps
    CHECK(gol::day_phase(150, 100) == doctest::Approx(0.5));
}

TEST_CASE("day_phase with zero day_length returns 0") {
    CHECK(gol::day_phase(42, 0) == doctest::Approx(0.0));
}

// ---- is_night ----

TEST_CASE("is_night returns true for second half of day") {
    CHECK_FALSE(gol::is_night(0, 100));
    CHECK_FALSE(gol::is_night(49, 100));
    CHECK(gol::is_night(50, 100));
    CHECK(gol::is_night(99, 100));
    CHECK_FALSE(gol::is_night(100, 100)); // wraps to day
}

TEST_CASE("is_night with zero day_length returns false") {
    CHECK_FALSE(gol::is_night(42, 0));
}

// ---- current_season ----

TEST_CASE("current_season cycles through four seasons") {
    int year = 1200;
    CHECK(gol::current_season(0, year) == gol::Season::Spring);
    CHECK(gol::current_season(299, year) == gol::Season::Spring);
    CHECK(gol::current_season(300, year) == gol::Season::Summer);
    CHECK(gol::current_season(599, year) == gol::Season::Summer);
    CHECK(gol::current_season(600, year) == gol::Season::Autumn);
    CHECK(gol::current_season(900, year) == gol::Season::Winter);
    CHECK(gol::current_season(1200, year) == gol::Season::Spring); // wraps
}

TEST_CASE("current_season with zero year_length returns Spring") {
    CHECK(gol::current_season(42, 0) == gol::Season::Spring);
}

// ---- seasonal_resource_factor ----

TEST_CASE("seasonal resource factors: spring > winter") {
    CHECK(gol::seasonal_resource_factor(gol::Season::Spring) >
          gol::seasonal_resource_factor(gol::Season::Winter));
    CHECK(gol::seasonal_resource_factor(gol::Season::Summer) >
          gol::seasonal_resource_factor(gol::Season::Autumn));
}

// ---- night_vision_factor ----

TEST_CASE("night_vision_factor halves at night") {
    CHECK(gol::night_vision_factor(0, 100) == doctest::Approx(1.0));
    CHECK(gol::night_vision_factor(50, 100) == doctest::Approx(0.5));
}

// ---- functional: cycle on vs off produces different outcomes ----

TEST_CASE("environmental cycle affects simulation outcome") {
    // Run two identical sims, one with cycle (day_length=100, year_length=1200),
    // one without (day_length=0, year_length=0). After 600 ticks, count food.

    auto make_world = [](int day_length, int year_length) {
        gol::WorldConfig cfg;
        cfg.width = 16;
        cfg.height = 16;
        cfg.seed = 42;
        cfg.initial_agents = 5;
        cfg.initial_food = 50;
        cfg.starting_energy = 150;
        cfg.food_spawn_rate = 0.1;
        cfg.mutation_rate = 0.01;
        cfg.day_length = day_length;
        cfg.year_length = year_length;
        return cfg;
    };

    auto count_food = [](const gol::World& w) {
        int total = 0;
        for (int y = 0; y < w.config().height; ++y) {
            for (int x = 0; x < w.config().width; ++x) {
                total += w.food().at(gol::Position{x, y});
            }
        }
        return total;
    };

    gol::WorldConfig cfg_cycle = make_world(100, 1200);
    gol::World world_cycle(cfg_cycle);
    world_cycle.init();
    gol::Simulation sim_cycle(world_cycle);

    gol::WorldConfig cfg_no_cycle = make_world(0, 0);
    gol::World world_no_cycle(cfg_no_cycle);
    world_no_cycle.init();
    gol::Simulation sim_no_cycle(world_no_cycle);

    for (int t = 0; t < 600; ++t) {
        sim_cycle.tick();
        sim_no_cycle.tick();
    }

    int food_cycle = count_food(world_cycle);
    int food_no_cycle = count_food(world_no_cycle);

    // The two runs should diverge due to seasonal effects on resource regen.
    // We just check they're not exactly equal (with identical seeds, any
    // difference must come from the cycle).
    CHECK(food_cycle != food_no_cycle);
}
