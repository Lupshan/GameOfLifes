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

// Gene byte indices within the genome.
static constexpr int GENE_VISION = 0;
static constexpr int GENE_SPEED = 1;
static constexpr int GENE_METABOLISM = 2;
static constexpr int GENE_REPRO_THRESHOLD = 3;
// Bytes 4-15: reserved for future traits.

struct Traits {
    int vision;
    int speed;
    int metabolism;
    int reproduction_threshold;
};

// Deterministic mapping from genome bytes to trait values.
Traits decode(const Genome& genome);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_TRAITS_HPP
