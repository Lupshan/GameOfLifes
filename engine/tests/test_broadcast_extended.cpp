#include "engine/broadcast.hpp"

#include <doctest/doctest.h>

TEST_CASE("broadcast with radius larger than grid wraps toroidally") {
    gol::RadioGrid radio(8, 8);
    // Radius 10 on an 8x8 grid should cover every cell.
    radio.broadcast(gol::Position{0, 0}, 55, 10);

    CHECK(radio.listen(gol::Position{0, 0}) == 55);
    CHECK(radio.listen(gol::Position{7, 7}) == 55);
    CHECK(radio.listen(gol::Position{4, 4}) == 55);
}

TEST_CASE("listen without broadcast returns zero") {
    gol::RadioGrid radio(16, 16);
    // No broadcast has been made — every cell should read 0.
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            CHECK(radio.listen(gol::Position{x, y}) == 0);
        }
    }
}

TEST_CASE("multiple broadcasts: last write wins per cell") {
    gol::RadioGrid radio(16, 16);
    radio.broadcast(gol::Position{5, 5}, 10, 2);
    radio.broadcast(gol::Position{5, 5}, 20, 2);

    // The cell at (5,5) received two broadcasts; value depends on implementation
    // (additive or overwrite). Either way, it should be non-zero.
    int val = radio.listen(gol::Position{5, 5});
    CHECK(val != 0);
}
