#include "engine/world_config.hpp"

#include <cstdio>
#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace {

std::filesystem::path write_temp_toml(const std::string& content) {
    auto p = std::filesystem::temp_directory_path() /
             ("gol_cfg_" + std::to_string(std::rand()) + ".toml");
    std::ofstream out(p);
    out << content;
    return p;
}

} // namespace

TEST_CASE("WorldConfig: load default values from minimal file") {
    auto p = write_temp_toml("seed = 7\n");
    auto cfg = gol::load_world_config(p);
    CHECK(cfg.seed == 7u);
    CHECK(cfg.width == 64);
    CHECK(cfg.height == 64);
    std::filesystem::remove(p);
}

TEST_CASE("WorldConfig: full file overrides every field") {
    auto p = write_temp_toml("width = 100\n"
                             "height = 80\n"
                             "seed = 999\n"
                             "initial_agents = 5\n"
                             "initial_food = 12\n"
                             "max_ticks = 1000\n"
                             "food_spawn_rate = 0.25\n"
                             "starting_energy = 250\n");
    auto cfg = gol::load_world_config(p);
    CHECK(cfg.width == 100);
    CHECK(cfg.height == 80);
    CHECK(cfg.seed == 999u);
    CHECK(cfg.initial_agents == 5);
    CHECK(cfg.initial_food == 12);
    CHECK(cfg.max_ticks == 1000);
    CHECK(cfg.food_spawn_rate == doctest::Approx(0.25));
    CHECK(cfg.starting_energy == 250);
    std::filesystem::remove(p);
}

TEST_CASE("WorldConfig: malformed file throws") {
    auto p = write_temp_toml("width = = oops\n");
    CHECK_THROWS_AS(gol::load_world_config(p), std::runtime_error);
    std::filesystem::remove(p);
}

TEST_CASE("WorldConfig: shipped default.toml loads") {
    auto p = std::filesystem::path("config") / "default.toml";
    if (!std::filesystem::exists(p)) {
        p = std::filesystem::path("../config") / "default.toml";
    }
    if (std::filesystem::exists(p)) {
        auto cfg = gol::load_world_config(p);
        CHECK(cfg.width > 0);
        CHECK(cfg.seed == 42u);
    }
}
