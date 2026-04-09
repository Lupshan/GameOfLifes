#include "engine/simulation.hpp"
#include "engine/stats.hpp"
#include "engine/version.hpp"
#include "engine/world.hpp"
#include "engine/world_config.hpp"

#include <filesystem>
#include <iostream>

static constexpr int DEFAULT_TICKS = 100;

int main() {
    std::cout << "engine version " << gol::version() << "\n";

    std::filesystem::path config_path = "engine/config/default.toml";
    gol::WorldConfig config = gol::load_world_config(config_path);

    int n_ticks = config.max_ticks > 0 ? config.max_ticks : DEFAULT_TICKS;

    gol::World world(config);
    world.init();

    std::cout << "world " << config.width << "x" << config.height
              << "  agents=" << config.initial_agents
              << "  food=" << config.initial_food
              << "  seed=" << config.seed << "\n";

    gol::Simulation sim(world);
    sim.run(n_ticks);

    return 0;
}
