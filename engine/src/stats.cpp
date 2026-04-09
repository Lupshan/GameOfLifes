#include "engine/stats.hpp"

#include <iostream>

namespace gol {

void print_stats(const World& world) {
    int alive_count = 0;
    int total_energy = 0;
    for (const auto& agent : world.agents()) {
        if (agent.alive) {
            ++alive_count;
            total_energy += agent.energy;
        }
    }

    int food_count = 0;
    int w = world.config().width;
    int h = world.config().height;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            food_count += world.food().at(Position{x, y});
        }
    }

    double avg_energy = 0.0;
    if (alive_count > 0) {
        avg_energy = static_cast<double>(total_energy) / alive_count;
    }

    std::cout << "tick " << world.tick_count() << "  alive=" << alive_count
              << "  food=" << food_count << "  avg_energy=" << avg_energy << "\n";
}

} // namespace gol
