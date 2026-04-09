#include "engine/rng.hpp"

#include <doctest/doctest.h>
#include <stdexcept>
#include <vector>

TEST_CASE("Rng: same seed produces same sequence") {
    gol::Rng a(12345);
    gol::Rng b(12345);
    for (int i = 0; i < 100; ++i) {
        CHECK(a.uniform_int(0, 1000) == b.uniform_int(0, 1000));
    }
}

TEST_CASE("Rng: different seeds diverge") {
    gol::Rng a(1);
    gol::Rng b(2);
    int diffs = 0;
    for (int i = 0; i < 50; ++i) {
        if (a.uniform_int(0, 1000000) != b.uniform_int(0, 1000000)) {
            ++diffs;
        }
    }
    CHECK(diffs > 40);
}

TEST_CASE("Rng: uniform_int respects bounds") {
    gol::Rng r(7);
    for (int i = 0; i < 1000; ++i) {
        int v = r.uniform_int(5, 10);
        CHECK(v >= 5);
        CHECK(v <= 10);
    }
}

TEST_CASE("Rng: uniform_int rejects min > max") {
    gol::Rng r(0);
    CHECK_THROWS_AS(r.uniform_int(5, 1), std::invalid_argument);
}

TEST_CASE("Rng: uniform_real in [0,1)") {
    gol::Rng r(0);
    for (int i = 0; i < 1000; ++i) {
        double v = r.uniform_real();
        CHECK(v >= 0.0);
        CHECK(v < 1.0);
    }
}

TEST_CASE("Rng: chance edge cases") {
    gol::Rng r(0);
    CHECK(r.chance(0.0) == false);
    CHECK(r.chance(-1.0) == false);
    CHECK(r.chance(1.0) == true);
    CHECK(r.chance(2.0) == true);
}

TEST_CASE("Rng: chance frequency approximates p") {
    gol::Rng r(123);
    int hits = 0;
    const int n = 10000;
    for (int i = 0; i < n; ++i) {
        if (r.chance(0.3)) {
            ++hits;
        }
    }
    double freq = static_cast<double>(hits) / n;
    CHECK(freq > 0.27);
    CHECK(freq < 0.33);
}

TEST_CASE("Rng: seed is exposed") {
    gol::Rng r(99);
    CHECK(r.seed() == 99u);
}
