#include "engine/broadcast.hpp"
#include "engine/memory_bank.hpp"

#include <doctest/doctest.h>

// ---- RadioGrid ----

TEST_CASE("broadcast and listen") {
    gol::RadioGrid radio(16, 16);
    radio.broadcast(gol::Position{5, 5}, 42, 2);

    CHECK(radio.listen(gol::Position{5, 5}) == 42);
    CHECK(radio.listen(gol::Position{4, 5}) == 42);
    CHECK(radio.listen(gol::Position{7, 5}) == 42);
    CHECK(radio.listen(gol::Position{8, 5}) == 0); // out of radius
}

TEST_CASE("radio grid resets") {
    gol::RadioGrid radio(16, 16);
    radio.broadcast(gol::Position{5, 5}, 99, 1);
    CHECK(radio.listen(gol::Position{5, 5}) == 99);

    radio.reset();
    CHECK(radio.listen(gol::Position{5, 5}) == 0);
}

// ---- MemoryBank ----

TEST_CASE("memory bank default zero") {
    gol::MemoryBank mem{};
    for (int i = 0; i < gol::MEMORY_BANK_SIZE; ++i) {
        CHECK(mem[static_cast<std::size_t>(i)] == 0);
    }
}

TEST_CASE("memory bank read write") {
    gol::MemoryBank mem{};
    mem[0] = 42;
    mem[15] = -1;
    CHECK(mem[0] == 42);
    CHECK(mem[15] == -1);
}
