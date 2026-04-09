#include "engine/snapshot.hpp"
#include "engine/snapshot_json.hpp"
#include "engine/world.hpp"

#include <doctest/doctest.h>

#include <cstdint>

static gol::WorldConfig tiny_config() {
    gol::WorldConfig cfg;
    cfg.width = 8;
    cfg.height = 8;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    cfg.starting_energy = 10;
    cfg.food_spawn_rate = 0.0;
    return cfg;
}

// ---- round-trip: empty world ----

TEST_CASE("snapshot round-trip empty world") {
    gol::World world(tiny_config());

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    CHECK(restored.version == 1);
    CHECK(restored.tick == 0);
    CHECK(restored.width == 8);
    CHECK(restored.height == 8);
    CHECK(restored.seed == 42);
    CHECK(restored.agents.empty());
    CHECK(restored.food.size() == 64);

    for (const auto& cell : restored.food) {
        CHECK(cell == 0);
    }
}

// ---- round-trip: world with agents and food ----

TEST_CASE("snapshot round-trip with agents and food") {
    gol::World world(tiny_config());
    world.spawn_agent(gol::Position{2, 3}, 50);
    world.spawn_agent(gol::Position{7, 7}, 25);
    world.food().set(gol::Position{0, 0}, 1);
    world.food().set(gol::Position{4, 5}, 1);
    world.food().set(gol::Position{7, 7}, 1);
    world.advance_tick();
    world.advance_tick();
    world.advance_tick();

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    CHECK(restored.tick == 3);
    CHECK(restored.agents.size() == 2);

    CHECK(restored.agents[0].id == 0);
    CHECK(restored.agents[0].pos.x == 2);
    CHECK(restored.agents[0].pos.y == 3);
    CHECK(restored.agents[0].energy == 50);
    CHECK(restored.agents[0].alive == true);

    CHECK(restored.agents[1].id == 1);
    CHECK(restored.agents[1].pos.x == 7);
    CHECK(restored.agents[1].pos.y == 7);
    CHECK(restored.agents[1].energy == 25);

    // Check food cells
    CHECK(restored.food[0 * 8 + 0] == 1); // (0,0)
    CHECK(restored.food[5 * 8 + 4] == 1); // (4,5)
    CHECK(restored.food[7 * 8 + 7] == 1); // (7,7)
    CHECK(restored.food[0 * 8 + 1] == 0); // (1,0) should be empty
}

// ---- version field present in JSON ----

TEST_CASE("snapshot JSON contains version field") {
    gol::World world(tiny_config());
    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);

    // Crude check: the string must contain "version": 1
    CHECK(json.find("\"version\": 1") != std::string::npos);
}

// ---- round-trip: all food cells set ----

TEST_CASE("snapshot round-trip full food grid") {
    gol::World world(tiny_config());
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            world.food().set(gol::Position{x, y}, 1);
        }
    }

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    for (std::size_t i = 0; i < restored.food.size(); ++i) {
        CHECK(restored.food[i] == 1);
    }
}

// ---- round-trip: non-square world ----

TEST_CASE("snapshot round-trip non-square world") {
    gol::WorldConfig cfg = tiny_config();
    cfg.width = 10;
    cfg.height = 3;
    gol::World world(cfg);
    world.food().set(gol::Position{9, 2}, 1);

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    CHECK(restored.width == 10);
    CHECK(restored.height == 3);
    CHECK(restored.food.size() == 30);
    CHECK(restored.food[2 * 10 + 9] == 1);
}

// ---- dead agent survives round-trip ----

TEST_CASE("snapshot round-trip dead agent") {
    gol::World world(tiny_config());
    world.spawn_agent(gol::Position{1, 1}, 0);
    world.agents()[0].alive = false;

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    CHECK(restored.agents.size() == 1);
    CHECK(restored.agents[0].alive == false);
    CHECK(restored.agents[0].energy == 0);
}

// ---- large world round-trip (stress bit packing) ----

TEST_CASE("snapshot round-trip large world") {
    gol::WorldConfig cfg;
    cfg.width = 100;
    cfg.height = 100;
    cfg.seed = 99;
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    cfg.food_spawn_rate = 0.0;
    cfg.starting_energy = 10;
    gol::World world(cfg);

    // Set a checkerboard pattern
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            if ((x + y) % 2 == 0) {
                world.food().set(gol::Position{x, y}, 1);
            }
        }
    }

    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);
    gol::Snapshot restored = gol::snapshot_from_json(json);

    CHECK(restored.food.size() == 10000);
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            std::size_t idx = static_cast<std::size_t>(y) * 100 + static_cast<std::size_t>(x);
            std::uint8_t expected = ((x + y) % 2 == 0) ? 1 : 0;
            CHECK(restored.food[idx] == expected);
        }
    }
}
