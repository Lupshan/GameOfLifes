#include "engine/genome.hpp"

namespace gol {

Genome random_genome(Rng& rng) {
    Genome g;
    for (auto& byte : g) {
        byte = static_cast<std::uint8_t>(rng.uniform_int(0, 255));
    }
    return g;
}

Genome mutate(const Genome& genome, Rng& rng, double rate) {
    Genome result = genome;
    for (auto& byte : result) {
        for (int bit = 0; bit < 8; ++bit) {
            if (rng.chance(rate)) {
                byte ^= static_cast<std::uint8_t>(1 << bit);
            }
        }
    }
    return result;
}

} // namespace gol
