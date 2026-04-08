#include "engine/position.hpp"

#include <doctest/doctest.h>
#include <unordered_set>

TEST_CASE("Position equality") {
    gol::Position a{1, 2};
    gol::Position b{1, 2};
    gol::Position c{2, 1};
    CHECK(a == b);
    CHECK(a != c);
}

TEST_CASE("Position is hashable") {
    std::unordered_set<gol::Position> s;
    s.insert({0, 0});
    s.insert({1, 0});
    s.insert({0, 0});
    CHECK(s.size() == 2);
}
