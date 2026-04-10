#ifndef GAMEOFLIFES_ENGINE_WORLD_CLOCK_HPP
#define GAMEOFLIFES_ENGINE_WORLD_CLOCK_HPP

#include <cstdint>

namespace gol {

enum class Season { Spring, Summer, Autumn, Winter };

// Pure functions: compute day/season from tick count and cycle lengths.

// Returns a value in [0, 1) representing the phase within the current day.
// 0.0 = dawn, 0.25 = noon, 0.5 = dusk, 0.75 = midnight.
double day_phase(std::uint64_t tick, int day_length) noexcept;

// True during the "night" half of the day (phase in [0.5, 1.0)).
bool is_night(std::uint64_t tick, int day_length) noexcept;

// Returns the current season based on tick and year length.
Season current_season(std::uint64_t tick, int year_length) noexcept;

// Resource spawn multiplier for the current season.
// Spring/Summer boost, Autumn/Winter reduce.
double seasonal_resource_factor(Season s) noexcept;

// Vision reduction factor at night: 0.5 (halved vision range).
double night_vision_factor(std::uint64_t tick, int day_length) noexcept;

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_WORLD_CLOCK_HPP
