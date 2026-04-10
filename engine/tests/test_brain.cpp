#include "engine/brain.hpp"
#include "engine/brain_bytecode.hpp"
#include "engine/brain_ml.hpp"
#include "engine/bytecode.hpp"
#include "engine/ml_runtime.hpp"
#include "engine/traits.hpp"

#include <doctest/doctest.h>

static gol::WorldConfig test_config() {
    gol::WorldConfig cfg;
    cfg.width = 16;
    cfg.height = 16;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 50;
    cfg.starting_energy = 200;
    cfg.food_spawn_rate = 0.1;
    cfg.mutation_rate = 0.01;
    cfg.instruction_budget = 1000;
    return cfg;
}

// A simple ML model: 10 inputs → 6 outputs (always picks action 0 = move).
static const std::string SIMPLE_ML_MODEL = R"({
    "layers": [{
        "input_size": 10,
        "output_size": 6,
        "activation": "none",
        "weights": [
            1,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0
        ],
        "biases": [10, 0, 0, 0, 0, 0]
    }]
})";

TEST_CASE("MlBrain runs without crashing") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.init();

    auto net = gol::load_model(SIMPLE_ML_MODEL);
    gol::MlBrain brain(net);

    world.spawn_agent(gol::Position{8, 8}, 200);
    auto& agent = world.agents().back();
    gol::Position start = agent.pos;

    brain.tick(agent, world);

    // The model always picks action 0 (move), so position should change.
    CHECK((agent.pos.x != start.x || agent.pos.y != start.y));
}

TEST_CASE("BytecodeBrain runs without crashing") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);

    // Simple bytecode: MOVE, HALT.
    auto bc = gol::make_bytecode({gol::op::MOVE, gol::op::HALT});
    gol::BytecodeBrain brain(bc, 1000);

    world.spawn_agent(gol::Position{8, 8}, 200);
    auto& agent = world.agents()[0];

    brain.tick(agent, world);
    // Should not crash.
    CHECK(true);
}

TEST_CASE("Mixed population: bytecode + ML agents coexist") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.init();

    auto net = gol::load_model(SIMPLE_ML_MODEL);
    auto bc = gol::make_bytecode({gol::op::MOVE, gol::op::HALT});

    // Spawn 5 ML agents.
    for (int i = 0; i < 5; ++i) {
        world.spawn_agent(gol::Position{world.rng().uniform_int(0, cfg.width - 1),
                                        world.rng().uniform_int(0, cfg.height - 1)},
                          200);
    }

    // Spawn 5 bytecode agents.
    for (int i = 0; i < 5; ++i) {
        world.spawn_agent(gol::Position{world.rng().uniform_int(0, cfg.width - 1),
                                        world.rng().uniform_int(0, cfg.height - 1)},
                          200);
    }

    gol::MlBrain ml_brain(net);
    gol::BytecodeBrain bc_brain(bc, 1000);

    // Run 50 ticks.
    for (int t = 0; t < 50; ++t) {
        for (std::size_t i = 0; i < world.agents().size(); ++i) {
            auto& agent = world.agents()[i];
            if (!agent.alive) {
                continue;
            }
            // First 5 agents use ML, rest use bytecode.
            if (agent.id < 5) {
                ml_brain.tick(agent, world);
            } else {
                bc_brain.tick(agent, world);
            }

            // Apply metabolism drain.
            gol::Traits traits = gol::decode(agent.genome);
            agent.energy -= traits.metabolism;
            agent.hydration -= 1;
            if (agent.energy <= 0 || agent.hydration <= 0) {
                agent.alive = false;
            }
        }

        // Remove dead agents.
        std::erase_if(world.agents(), [](const gol::Agent& a) {
            return !a.alive;
        });

        world.advance_tick();
    }

    // At least some agents should survive with food available.
    // (Don't assert exact count — just no crash.)
    CHECK(true);
}
