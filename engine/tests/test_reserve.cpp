#include "engine/brain_learning.hpp"
#include "engine/ml_runtime.hpp"
#include "engine/reserve.hpp"
#include "engine/traits.hpp"

#include <doctest/doctest.h>

// ---- Reserve ----

TEST_CASE("inside_reserve checks bounds") {
    gol::Reserve r{"R1", gol::Position{10, 10}, 20, 20, 50, 100};
    CHECK(gol::inside_reserve(r, gol::Position{15, 15}));
    CHECK(gol::inside_reserve(r, gol::Position{10, 10}));
    CHECK_FALSE(gol::inside_reserve(r, gol::Position{9, 15}));
    CHECK_FALSE(gol::inside_reserve(r, gol::Position{30, 15}));
    CHECK_FALSE(gol::inside_reserve(r, gol::Position{15, 30}));
}

// ---- LearningBrain ----

static const std::string LEARNING_MODEL = R"({
    "layers": [{
        "input_size": 10,
        "output_size": 6,
        "activation": "relu",
        "weights": [
            0.1,0,0,0,0,0,0,0,0,0,
            0,0.1,0,0,0,0,0,0,0,0,
            0,0,0.1,0,0,0,0,0,0,0,
            0,0,0,0.1,0,0,0,0,0,0,
            0,0,0,0,0.1,0,0,0,0,0,
            0,0,0,0,0,0.1,0,0,0,0
        ],
        "biases": [1, 0, 0, 0, 0, 0]
    }]
})";

TEST_CASE("LearningBrain runs without crashing") {
    gol::WorldConfig cfg;
    cfg.width = 32;
    cfg.height = 32;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 100;
    cfg.starting_energy = 200;
    cfg.food_spawn_rate = 0.1;
    cfg.mutation_rate = 0.01;
    gol::World world(cfg);
    world.init();

    auto net = gol::load_model(LEARNING_MODEL);
    gol::LearningBrain brain(net, 0.01, 10);

    world.spawn_agent(gol::Position{16, 16}, 200);

    for (int t = 0; t < 100; ++t) {
        auto& agent = world.agents()[0];
        if (!agent.alive) {
            break;
        }

        brain.tick(agent, world);

        gol::Traits traits = gol::decode(agent.genome);
        agent.energy -= traits.metabolism;
        agent.hydration -= 1;
        if (agent.energy <= 0 || agent.hydration <= 0) {
            agent.alive = false;
        }
    }

    CHECK(true); // No crash.
}
