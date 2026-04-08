#include "engine/version.hpp"

#include <doctest/doctest.h>

TEST_CASE("trivial sanity check") {
    CHECK(1 + 1 == 2);
}

TEST_CASE("engine version is non-empty") {
    CHECK_FALSE(gol::version().empty());
}
