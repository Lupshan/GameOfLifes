#include "engine/world_clock.hpp"

namespace gol {

double day_phase(std::uint64_t tick, int day_length) {
    if (day_length <= 0) {
        return 0.0;
    }
    return static_cast<double>(tick % static_cast<std::uint64_t>(day_length)) /
           static_cast<double>(day_length);
}

bool is_night(std::uint64_t tick, int day_length) {
    if (day_length <= 0) {
        return false;
    }
    return day_phase(tick, day_length) >= 0.5;
}

Season current_season(std::uint64_t tick, int year_length) {
    if (year_length <= 0) {
        return Season::Spring;
    }
    int quarter_len = year_length / 4;
    if (quarter_len <= 0) {
        return Season::Spring;
    }
    int pos = static_cast<int>(tick % static_cast<std::uint64_t>(year_length));
    int quarter = pos / quarter_len;
    if (quarter >= 4) {
        quarter = 3;
    }
    return static_cast<Season>(quarter);
}

double seasonal_resource_factor(Season s) {
    switch (s) {
    case Season::Spring:
        return 1.5;
    case Season::Summer:
        return 1.2;
    case Season::Autumn:
        return 0.8;
    case Season::Winter:
        return 0.4;
    }
    return 1.0;
}

double night_vision_factor(std::uint64_t tick, int day_length) {
    if (is_night(tick, day_length)) {
        return 0.5;
    }
    return 1.0;
}

} // namespace gol
