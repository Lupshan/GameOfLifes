#include "engine/rng.hpp"

#include <stdexcept>

namespace gol {

Rng::Rng(std::uint64_t seed) noexcept : seed_(seed), engine_(seed) {
}

int Rng::uniform_int(int min_inclusive, int max_inclusive) {
    if (min_inclusive > max_inclusive) {
        throw std::invalid_argument("uniform_int: min > max");
    }
    std::uniform_int_distribution<int> dist(min_inclusive, max_inclusive);
    return dist(engine_);
}

double Rng::uniform_real() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

bool Rng::chance(double p) {
    if (p <= 0.0) {
        return false;
    }
    if (p >= 1.0) {
        return true;
    }
    return uniform_real() < p;
}

std::uint64_t Rng::seed() const noexcept {
    return seed_;
}

} // namespace gol
