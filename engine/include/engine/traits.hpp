#ifndef GAMEOFLIFES_ENGINE_TRAITS_HPP
#define GAMEOFLIFES_ENGINE_TRAITS_HPP

#include "engine/genome.hpp"

namespace gol {

// Trait value bounds.
static constexpr int VISION_MIN = 1;
static constexpr int VISION_MAX = 5;
static constexpr int SPEED_MIN = 1;
static constexpr int SPEED_MAX = 3;
static constexpr int METABOLISM_MIN = 1;
static constexpr int METABOLISM_MAX = 5;
static constexpr int REPRO_THRESHOLD_MIN = 50;
static constexpr int REPRO_THRESHOLD_MAX = 200;
static constexpr int SWIM_MIN = 0;
static constexpr int SWIM_MAX = 3;
static constexpr int FORAGE_MIN = 1;
static constexpr int FORAGE_MAX = 3;

// Gene byte indices within the genome.
static constexpr int GENE_VISION = 0;
static constexpr int GENE_SPEED = 1;
static constexpr int GENE_METABOLISM = 2;
static constexpr int GENE_REPRO_THRESHOLD = 3;
static constexpr int GENE_SWIM = 4;
static constexpr int GENE_FORAGE = 5;
// Bytes 6-15: reserved for future traits.

struct Traits {
    int vision;
    int speed;
    int metabolism;
    int reproduction_threshold;
    int swim;              // 0 = can't swim, 1-3 = water movement cost reduction
    int forage_efficiency; // 1-3 = multiplier for food/water/mineral pickup
};

// Deterministic mapping from genome bytes to trait values.
Traits decode(const Genome& genome);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_TRAITS_HPP
