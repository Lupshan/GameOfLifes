#include "engine/world_config.hpp"

#include <stdexcept>
#include <string>
#include <toml++/toml.hpp>

namespace gol {

namespace {

template <typename T> T get_or(const toml::table& tbl, std::string_view key, T fallback) {
    auto node = tbl[key];
    if (auto v = node.value<T>()) {
        return *v;
    }
    return fallback;
}

} // namespace

WorldConfig load_world_config(const std::filesystem::path& path) {
    toml::table tbl;
    try {
        tbl = toml::parse_file(path.string());
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::string("world_config: parse error: ") + e.what());
    }

    WorldConfig cfg;
    cfg.width = get_or<int64_t>(tbl, "width", cfg.width);
    cfg.height = get_or<int64_t>(tbl, "height", cfg.height);
    cfg.seed =
        static_cast<std::uint64_t>(get_or<int64_t>(tbl, "seed", static_cast<int64_t>(cfg.seed)));
    cfg.initial_agents = get_or<int64_t>(tbl, "initial_agents", cfg.initial_agents);
    cfg.initial_food = get_or<int64_t>(tbl, "initial_food", cfg.initial_food);
    cfg.max_ticks = get_or<int64_t>(tbl, "max_ticks", cfg.max_ticks);
    cfg.food_spawn_rate = get_or<double>(tbl, "food_spawn_rate", cfg.food_spawn_rate);
    cfg.starting_energy = get_or<int64_t>(tbl, "starting_energy", cfg.starting_energy);
    cfg.mutation_rate = get_or<double>(tbl, "mutation_rate", cfg.mutation_rate);
    cfg.day_length = get_or<int64_t>(tbl, "day_length", cfg.day_length);
    cfg.year_length = get_or<int64_t>(tbl, "year_length", cfg.year_length);
    return cfg;
}

} // namespace gol
