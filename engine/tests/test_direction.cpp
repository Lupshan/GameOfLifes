#include "engine/direction.hpp"

#include <doctest/doctest.h>

TEST_CASE("Direction offsets are unit vectors") {
    CHECK(gol::to_offset(gol::Direction::North) == gol::Position{0, -1});
    CHECK(gol::to_offset(gol::Direction::East) == gol::Position{1, 0});
    CHECK(gol::to_offset(gol::Direction::South) == gol::Position{0, 1});
    CHECK(gol::to_offset(gol::Direction::West) == gol::Position{-1, 0});
}

TEST_CASE("turn_left cycles N->W->S->E->N") {
    CHECK(gol::turn_left(gol::Direction::North) == gol::Direction::West);
    CHECK(gol::turn_left(gol::Direction::West) == gol::Direction::South);
    CHECK(gol::turn_left(gol::Direction::South) == gol::Direction::East);
    CHECK(gol::turn_left(gol::Direction::East) == gol::Direction::North);
}

TEST_CASE("turn_right cycles N->E->S->W->N") {
    CHECK(gol::turn_right(gol::Direction::North) == gol::Direction::East);
    CHECK(gol::turn_right(gol::Direction::East) == gol::Direction::South);
    CHECK(gol::turn_right(gol::Direction::South) == gol::Direction::West);
    CHECK(gol::turn_right(gol::Direction::West) == gol::Direction::North);
}

TEST_CASE("four turn_left calls return to start") {
    auto d = gol::Direction::North;
    for (int i = 0; i < 4; ++i) {
        d = gol::turn_left(d);
    }
    CHECK(d == gol::Direction::North);
}
