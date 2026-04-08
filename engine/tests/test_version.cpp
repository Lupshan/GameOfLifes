#include <doctest/doctest.h>

#include "engine/version.hpp"

TEST_CASE("trivial sanity check")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE("engine version is non-empty")
{
    CHECK_FALSE(gol::version().empty());
}
