#ifndef GAMEOFLIFES_ENGINE_WORLD_CONFIG_HPP
#define GAMEOFLIFES_ENGINE_WORLD_CONFIG_HPP

#include <cstdint>
#include <filesystem>

namespace gol {

struct WorldConfig {
    int width = 64;
    int height = 64;
    std::uint64_t seed = 1;
    int initial_agents = 16;
    int initial_food = 64;
    int max_ticks = 0; // 0 = unbounded
    double food_spawn_rate = 0.05;
    int starting_energy = 100;
    int starting_hydration = 100;
    double mutation_rate = 0.01;
    int day_length = 100;          // ticks per day (0 = no day/night cycle)
    int year_length = 1200;        // ticks per year (0 = no seasons)
    int instruction_budget = 1000; // VM instructions per agent per tick
};

WorldConfig load_world_config(const std::filesystem::path& path);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_WORLD_CONFIG_HPP
