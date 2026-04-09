#include "engine/simulation.hpp"
#include "engine/snapshot_json.hpp"
#include "engine/stats.hpp"
#include "engine/version.hpp"
#include "engine/world.hpp"
#include "engine/world_config.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static constexpr int DEFAULT_TICKS = 100;

static void dump_snapshot(const gol::World& world, const std::filesystem::path& dir) {
    gol::Snapshot snap = gol::capture_snapshot(world);
    std::string json = gol::snapshot_to_json(snap);

    std::ostringstream filename;
    filename << "tick_" << std::setw(6) << std::setfill('0') << world.tick_count() << ".json";

    std::filesystem::path path = dir / filename.str();
    std::ofstream out(path);
    if (!out) {
        std::cerr << "error: cannot write " << path << "\n";
        return;
    }
    out << json << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "engine version " << gol::version() << "\n";

    std::filesystem::path config_path = "engine/config/default.toml";
    std::filesystem::path snapshot_dir;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dump-snapshots" && i + 1 < argc) {
            snapshot_dir = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        } else {
            std::cerr << "usage: gameoflifes_engine [--config <path>] [--dump-snapshots <dir>]\n";
            return EXIT_FAILURE;
        }
    }

    gol::WorldConfig config = gol::load_world_config(config_path);

    int n_ticks = config.max_ticks > 0 ? config.max_ticks : DEFAULT_TICKS;

    gol::World world(config);
    world.init();

    std::cout << "world " << config.width << "x" << config.height
              << "  agents=" << config.initial_agents << "  food=" << config.initial_food
              << "  seed=" << config.seed << "\n";

    if (!snapshot_dir.empty()) {
        std::filesystem::create_directories(snapshot_dir);
        dump_snapshot(world, snapshot_dir);
    }

    gol::Simulation sim(world);
    for (int t = 0; t < n_ticks; ++t) {
        sim.tick();
        if (!snapshot_dir.empty()) {
            dump_snapshot(world, snapshot_dir);
        }
    }

    return 0;
}
