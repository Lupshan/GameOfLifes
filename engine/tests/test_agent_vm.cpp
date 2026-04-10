#include "engine/agent_vm.hpp"
#include "engine/bytecode.hpp"
#include "engine/direction.hpp"
#include "engine/traits.hpp"
#include "engine/vm.hpp"
#include "engine/world.hpp"

#include <cstdint>
#include <doctest/doctest.h>
#include <vector>

static gol::WorldConfig test_config() {
    gol::WorldConfig cfg;
    cfg.width = 8;
    cfg.height = 8;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 0;
    cfg.starting_energy = 100;
    cfg.food_spawn_rate = 0.0;
    cfg.mutation_rate = 0.01;
    return cfg;
}

// ---- PERCEIVE ----

TEST_CASE("VM intrinsic: PERCEIVE pushes 4 values") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.food().set(gol::Position{4, 4}, 1);
    world.terrain().set(gol::Position{4, 4}, gol::Terrain::Forest);
    world.spawn_agent(gol::Position{4, 4}, 100);
    world.rebuild_caches();

    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::PERCEIVE, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_size() == 4);
    // Top = agents_here, then water, food, terrain (bottom to top per spec)
    int agents_here = vm.stack_top();
    CHECK(agents_here >= 1); // at least the agent itself
}

// ---- MOVE ----

TEST_CASE("VM intrinsic: MOVE moves agent forward") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.spawn_agent(gol::Position{4, 4}, 100);

    auto& agent = world.agents()[0];
    agent.facing = gol::Direction::East;
    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::MOVE, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(agent.pos.x == 5);
    CHECK(agent.pos.y == 4);
}

// ---- TURN_LEFT / TURN_RIGHT ----

TEST_CASE("VM intrinsic: TURN_LEFT and TURN_RIGHT") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.spawn_agent(gol::Position{4, 4}, 100);

    auto& agent = world.agents()[0];
    agent.facing = gol::Direction::North;
    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::TURN_LEFT, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);
    CHECK(agent.facing == gol::Direction::West);

    agent.facing = gol::Direction::North;
    std::vector<std::uint8_t> code2 = {gol::op::TURN_RIGHT, gol::op::HALT};
    auto bc2 = gol::make_bytecode(code2);
    vm.load(bc2);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);
    CHECK(agent.facing == gol::Direction::East);
}

// ---- EAT ----

TEST_CASE("VM intrinsic: EAT consumes food") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.food().set(gol::Position{4, 4}, 1);
    world.spawn_agent(gol::Position{4, 4}, 50);

    auto& agent = world.agents()[0];
    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::EAT, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_top() > 0);                        // gained some energy
    CHECK(agent.energy > 50);                         // energy increased
    CHECK(world.food().at(gol::Position{4, 4}) == 0); // food consumed
}

// ---- DRINK ----

TEST_CASE("VM intrinsic: DRINK consumes water") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.water().set(gol::Position{4, 4}, 1);
    world.spawn_agent(gol::Position{4, 4}, 100);
    world.agents()[0].hydration = 50;

    auto& agent = world.agents()[0];
    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::DRINK, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_top() > 0);
    CHECK(agent.hydration > 50);
}

// ---- MY_ENERGY / MY_HYDRATION ----

TEST_CASE("VM intrinsic: MY_ENERGY and MY_HYDRATION") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);
    world.spawn_agent(gol::Position{4, 4}, 77);
    world.agents()[0].hydration = 33;

    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::MY_ENERGY, gol::op::MY_HYDRATION, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_size() == 2);
    CHECK(vm.stack_top() == 33); // hydration on top
}

// ---- MY_TRAIT ----

TEST_CASE("VM intrinsic: MY_TRAIT reads trait value") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);

    gol::Genome g{};
    g[gol::GENE_METABOLISM] = 0; // metabolism = 1
    world.spawn_agent(gol::Position{4, 4}, 100, g);

    gol::AgentIntrinsicHandler handler(0, world);

    // MY_TRAIT with trait_id=2 (metabolism)
    std::vector<std::uint8_t> code = {gol::op::MY_TRAIT, 2, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_top() == gol::METABOLISM_MIN);
}

// ---- REPRODUCE ----

TEST_CASE("VM intrinsic: REPRODUCE when energy sufficient") {
    gol::WorldConfig cfg = test_config();
    gol::World world(cfg);

    gol::Genome g{};
    g[gol::GENE_REPRO_THRESHOLD] = 0; // threshold = 50
    world.spawn_agent(gol::Position{4, 4}, 100, g);

    // Note: reproduce spawns a child which may invalidate references.
    // Use the handler bound to agent[0] but check by ID after.
    gol::AgentIntrinsicHandler handler(0, world);

    std::vector<std::uint8_t> code = {gol::op::REPRODUCE, gol::op::HALT};
    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);
    vm.set_intrinsic_handler(&handler);
    vm.run(100);

    CHECK(vm.stack_top() == 1); // success
    CHECK(world.agents().size() == 2);
    // Find parent by id=0.
    int parent_energy = -1;
    for (const auto& a : world.agents()) {
        if (a.id == 0) {
            parent_energy = a.energy;
        }
    }
    CHECK(parent_energy == 50); // lost half
}

// ---- Random walker bot (hand-written bytecode) ----

TEST_CASE("VM: random walker bot runs for 100 ticks without crashing") {
    // Bot logic: each tick: TURN_RIGHT, MOVE, EAT, DRINK, HALT
    // (Always turns right then moves — simple but functional.)
    std::vector<std::uint8_t> code = {
        gol::op::TURN_RIGHT,
        gol::op::MOVE,
        gol::op::EAT,
        gol::op::POP, // discard EAT result
        gol::op::DRINK,
        gol::op::POP, // discard DRINK result
        gol::op::HALT,
    };

    gol::WorldConfig cfg;
    cfg.width = 16;
    cfg.height = 16;
    cfg.seed = 42;
    cfg.initial_agents = 0;
    cfg.initial_food = 50;
    cfg.starting_energy = 200;
    cfg.food_spawn_rate = 0.1;
    cfg.mutation_rate = 0.0;
    cfg.instruction_budget = 1000;
    gol::World world(cfg);
    world.init();

    // Spawn an agent.
    gol::Genome g{};
    g[gol::GENE_METABOLISM] = 0; // low metabolism
    world.spawn_agent(gol::Position{8, 8}, 200, g);

    auto bc = gol::make_bytecode(code);
    gol::Vm vm;
    vm.load(bc);

    auto& agent = world.agents()[0];
    gol::AgentIntrinsicHandler handler(0, world);
    vm.set_intrinsic_handler(&handler);

    // Run for 100 ticks.
    for (int tick = 0; tick < 100; ++tick) {
        if (!agent.alive) {
            break;
        }
        vm.reset_for_tick();
        gol::VmStatus status = vm.run(cfg.instruction_budget);
        CHECK((status == gol::VmStatus::Halted || status == gol::VmStatus::BudgetExhausted));

        // Apply metabolism drain (the VM handles movement cost, but not passive drain).
        gol::Traits traits = gol::decode(agent.genome);
        agent.energy -= traits.metabolism;
        agent.hydration -= 1;
        if (agent.energy <= 0 || agent.hydration <= 0) {
            agent.alive = false;
        }
    }

    // The bot should have survived some ticks (food available).
    // We don't assert exact values, just that it ran without crashing.
    CHECK(true);
}
