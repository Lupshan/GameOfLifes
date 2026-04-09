#include "engine/traits.hpp"

namespace gol {

// Map a 0-255 byte to a clamped integer range [lo, hi].
static int map_byte(std::uint8_t byte, int lo, int hi) {
    // Linear interpolation: byte 0 → lo, byte 255 → hi.
    return lo + static_cast<int>(static_cast<double>(byte) / 255.0 * (hi - lo) + 0.5);
}

Traits decode(const Genome& genome) {
    Traits t;
    t.vision = map_byte(genome[GENE_VISION], VISION_MIN, VISION_MAX);
    t.speed = map_byte(genome[GENE_SPEED], SPEED_MIN, SPEED_MAX);
    t.metabolism = map_byte(genome[GENE_METABOLISM], METABOLISM_MIN, METABOLISM_MAX);
    t.reproduction_threshold =
        map_byte(genome[GENE_REPRO_THRESHOLD], REPRO_THRESHOLD_MIN, REPRO_THRESHOLD_MAX);
    return t;
}

} // namespace gol
