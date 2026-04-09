#ifndef GAMEOFLIFES_ENGINE_RNG_HPP
#define GAMEOFLIFES_ENGINE_RNG_HPP

#include <cstdint>
#include <random>

namespace gol {

class Rng {
  public:
    explicit Rng(std::uint64_t seed) noexcept;

    int uniform_int(int min_inclusive, int max_inclusive);
    double uniform_real();
    bool chance(double p);

    std::uint64_t seed() const noexcept;

  private:
    std::uint64_t seed_;
    std::mt19937_64 engine_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_RNG_HPP
