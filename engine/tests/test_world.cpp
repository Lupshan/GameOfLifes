#include "engine/world.hpp"

#include <doctest/doctest.h>

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

// ---- food ----

TEST_CASE("food place and consume") {
    gol::World world(tiny_config());
    gol::Position p{3, 4};

    CHECK(world.food().at(p) == 0);
    world.food().set(p, 1);
    CHECK(world.food().at(p) == 1);

    // consume
    world.food().set(p, 0);
    CHECK(world.food().at(p) == 0);
}

// ---- agent move (toroidal) ----

TEST_CASE("agent moves toroidally") {
    gol::World world(tiny_config());
    world.spawn_agent(gol::Position{7, 0}, 10);

    gol::Agent& agent = world.agents()[0];
    // move east from x=7 on width=8 → wraps to x=0
    gol::Position new_pos = world.wrap_position(gol::Position{agent.pos.x + 1, agent.pos.y});
    agent.pos = new_pos;

    CHECK(agent.pos.x == 0);
    CHECK(agent.pos.y == 0);
}

TEST_CASE("agent moves toroidally negative") {
    gol::World world(tiny_config());
    world.spawn_agent(gol::Position{0, 0}, 10);

    gol::Agent& agent = world.agents()[0];
    // move west from x=0 → wraps to x=7
    gol::Position new_pos = world.wrap_position(gol::Position{agent.pos.x - 1, agent.pos.y});
    agent.pos = new_pos;

    CHECK(agent.pos.x == 7);
    CHECK(agent.pos.y == 0);
}

// ---- agent reap ----

TEST_CASE("dead agents are reapable with erase_if") {
    gol::World world(tiny_config());
    world.spawn_agent(gol::Position{0, 0}, 10);
    world.spawn_agent(gol::Position{1, 1}, 0);

    world.agents()[1].alive = false;

    std::erase_if(world.agents(), [](const gol::Agent& a) {
        return !a.alive;
    });

    CHECK(world.agents().size() == 1);
    CHECK(world.agents()[0].id == 0);
}

// ---- init ----

TEST_CASE("init places food and agents") {
    gol::WorldConfig cfg = tiny_config();
    cfg.initial_food = 10;
    cfg.initial_agents = 4;
    gol::World world(cfg);
    world.init();

    CHECK(world.agents().size() == 4);

    int food_count = 0;
    for (int y = 0; y < cfg.height; ++y) {
        for (int x = 0; x < cfg.width; ++x) {
            food_count += world.food().at(gol::Position{x, y});
        }
    }
    // could be less than 10 if some land on same cell, but > 0
    CHECK(food_count > 0);
}

// ---- spawn gives sequential ids ----

TEST_CASE("spawn_agent assigns sequential ids") {
    gol::World world(tiny_config());
    std::uint64_t id0 = world.spawn_agent(gol::Position{0, 0}, 5);
    std::uint64_t id1 = world.spawn_agent(gol::Position{1, 1}, 5);

    CHECK(id0 == 0);
    CHECK(id1 == 1);
}
