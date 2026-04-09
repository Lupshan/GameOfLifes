#ifndef GAMEOFLIFES_ENGINE_GENOME_HPP
#define GAMEOFLIFES_ENGINE_GENOME_HPP

#include "engine/rng.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace gol {

static constexpr std::size_t GENOME_LENGTH = 16;

using Genome = std::array<std::uint8_t, GENOME_LENGTH>;

// Generate a random genome.
Genome random_genome(Rng& rng);

// Mutate a genome by flipping individual bits with probability `rate`.
Genome mutate(const Genome& genome, Rng& rng, double rate);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_GENOME_HPP
