#ifndef GAMEOFLIFES_ENGINE_TERRAIN_HPP
#define GAMEOFLIFES_ENGINE_TERRAIN_HPP

#include <cstdint>

namespace gol {

enum class Terrain : std::uint8_t {
    Plain = 0,
    Forest = 1,
    Water = 2,
    Rock = 3,
};

// Movement cost multiplier for each terrain type.
// Agents spend this many moves to cross a cell.
int movement_cost(Terrain t);

// Food regeneration rate multiplier for each terrain type.
double food_regen_factor(Terrain t);

// Water regeneration rate multiplier for each terrain type.
double water_regen_factor(Terrain t);

// Mineral regeneration rate multiplier for each terrain type.
double mineral_regen_factor(Terrain t);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_TERRAIN_HPP
