#include "engine/grid.hpp"

#include <doctest/doctest.h>
#include <stdexcept>

TEST_CASE("wrap: positive in-bounds") {
    CHECK(gol::wrap(0, 10) == 0);
    CHECK(gol::wrap(5, 10) == 5);
    CHECK(gol::wrap(9, 10) == 9);
}

TEST_CASE("wrap: positive overflow") {
    CHECK(gol::wrap(10, 10) == 0);
    CHECK(gol::wrap(11, 10) == 1);
    CHECK(gol::wrap(25, 10) == 5);
}

TEST_CASE("wrap: negative") {
    CHECK(gol::wrap(-1, 10) == 9);
    CHECK(gol::wrap(-10, 10) == 0);
    CHECK(gol::wrap(-11, 10) == 9);
    CHECK(gol::wrap(-25, 10) == 5);
}

TEST_CASE("wrap: zero or negative size throws") {
    CHECK_THROWS_AS(gol::wrap(0, 0), std::invalid_argument);
    CHECK_THROWS_AS(gol::wrap(0, -1), std::invalid_argument);
}

TEST_CASE("Grid: construction and dimensions") {
    gol::Grid<int> g(4, 3);
    CHECK(g.width() == 4);
    CHECK(g.height() == 3);
}

TEST_CASE("Grid: invalid dimensions throw") {
    CHECK_THROWS_AS(gol::Grid<int>(0, 5), std::invalid_argument);
    CHECK_THROWS_AS(gol::Grid<int>(5, -1), std::invalid_argument);
}

TEST_CASE("Grid: set/at round-trip") {
    gol::Grid<int> g(4, 3, 0);
    g.set({1, 2}, 42);
    CHECK(g.at({1, 2}) == 42);
    CHECK(g.at({0, 0}) == 0);
}

TEST_CASE("Grid: toroidal access") {
    gol::Grid<int> g(4, 3, 0);
    g.set({1, 2}, 7);
    CHECK(g.at({1 + 4, 2 + 3}) == 7);
    CHECK(g.at({1 - 4, 2 - 3}) == 7);
    CHECK(g.at({-3, -1}) == 7);
}

TEST_CASE("Grid: iterators visit all cells") {
    gol::Grid<int> g(2, 2, 1);
    int sum = 0;
    for (auto v : g) {
        sum += v;
    }
    CHECK(sum == 4);
}
