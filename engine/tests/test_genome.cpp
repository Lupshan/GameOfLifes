#include "engine/genome.hpp"
#include "engine/traits.hpp"

#include <cmath>
#include <cstdint>
#include <doctest/doctest.h>

// ---- decode is deterministic ----

TEST_CASE("decode produces same traits for same genome") {
    gol::Genome g{};
    g[gol::GENE_VISION] = 128;
    g[gol::GENE_SPEED] = 200;
    g[gol::GENE_METABOLISM] = 50;
    g[gol::GENE_REPRO_THRESHOLD] = 255;

    gol::Traits t1 = gol::decode(g);
    gol::Traits t2 = gol::decode(g);

    CHECK(t1.vision == t2.vision);
    CHECK(t1.speed == t2.speed);
    CHECK(t1.metabolism == t2.metabolism);
    CHECK(t1.reproduction_threshold == t2.reproduction_threshold);
}

// ---- trait clamping: min values ----

TEST_CASE("decode clamps to minimum at byte 0") {
    gol::Genome g{};
    // All gene bytes = 0 → minimum traits.
    gol::Traits t = gol::decode(g);

    CHECK(t.vision == gol::VISION_MIN);
    CHECK(t.speed == gol::SPEED_MIN);
    CHECK(t.metabolism == gol::METABOLISM_MIN);
    CHECK(t.reproduction_threshold == gol::REPRO_THRESHOLD_MIN);
}

// ---- trait clamping: max values ----

TEST_CASE("decode clamps to maximum at byte 255") {
    gol::Genome g{};
    g.fill(255);
    gol::Traits t = gol::decode(g);

    CHECK(t.vision == gol::VISION_MAX);
    CHECK(t.speed == gol::SPEED_MAX);
    CHECK(t.metabolism == gol::METABOLISM_MAX);
    CHECK(t.reproduction_threshold == gol::REPRO_THRESHOLD_MAX);
}

// ---- trait mid values ----

TEST_CASE("decode mid-range byte gives mid-range trait") {
    gol::Genome g{};
    g[gol::GENE_VISION] = 128;
    gol::Traits t = gol::decode(g);

    // 128/255 * (5-1) + 1 ≈ 3
    CHECK(t.vision >= gol::VISION_MIN);
    CHECK(t.vision <= gol::VISION_MAX);
    CHECK(t.vision == 3);
}

// ---- random_genome produces valid genome ----

TEST_CASE("random_genome produces 16-byte genome") {
    gol::Rng rng(42);
    gol::Genome g = gol::random_genome(rng);

    CHECK(g.size() == gol::GENOME_LENGTH);
}

// ---- random_genome is deterministic with same seed ----

TEST_CASE("random_genome is deterministic with same seed") {
    gol::Rng rng1(42);
    gol::Rng rng2(42);

    gol::Genome g1 = gol::random_genome(rng1);
    gol::Genome g2 = gol::random_genome(rng2);

    CHECK(g1 == g2);
}

// ---- mutate with rate 0 produces identical genome ----

TEST_CASE("mutate rate 0 returns identical genome") {
    gol::Rng rng(42);
    gol::Genome g = gol::random_genome(rng);
    gol::Genome m = gol::mutate(g, rng, 0.0);

    CHECK(g == m);
}

// ---- mutate with rate 1 flips all bits ----

TEST_CASE("mutate rate 1 flips all bits") {
    gol::Rng rng(42);
    gol::Genome g{};
    g.fill(0);
    gol::Genome m = gol::mutate(g, rng, 1.0);

    // Every byte should be 0xFF (all bits flipped from 0).
    for (auto byte : m) {
        CHECK(byte == 0xFF);
    }
}

// ---- statistical mutation test ----

TEST_CASE("mutate statistical test") {
    // With rate = 0.1, over 1000 genomes, count total bit flips.
    // Expected flips per genome: 16 * 8 * 0.1 = 12.8
    // Over 1000 genomes: expected ~12800 flips.
    // Tolerance: ±20% → [10240, 15360].

    gol::Rng rng(123);
    gol::Genome base{};
    base.fill(0);

    int total_flips = 0;
    constexpr int TRIALS = 1000;

    for (int trial = 0; trial < TRIALS; ++trial) {
        gol::Genome m = gol::mutate(base, rng, 0.1);
        for (std::size_t i = 0; i < gol::GENOME_LENGTH; ++i) {
            // Count set bits (flips from 0).
            std::uint8_t byte = m[i];
            while (byte > 0) {
                total_flips += byte & 1;
                byte >>= 1;
            }
        }
    }

    double expected = TRIALS * gol::GENOME_LENGTH * 8 * 0.1;
    double lower = expected * 0.8;
    double upper = expected * 1.2;

    CHECK(total_flips > static_cast<int>(lower));
    CHECK(total_flips < static_cast<int>(upper));
}
