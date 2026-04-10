#include "engine/lineage_log.hpp"

#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("lineage log writes birth events") {
    std::filesystem::path tmp = std::filesystem::temp_directory_path() / "test_lineage.jsonl";
    {
        gol::LineageLog log(tmp);
        CHECK(log.enabled());

        gol::Genome g{};
        g[0] = 0xAB;
        log.log_birth(10, 1, 0, 2, g);
        log.log_death(20, 1, "starvation");
    }

    // Read and verify.
    std::ifstream f(tmp);
    std::string line1;
    std::string line2;
    std::getline(f, line1);
    std::getline(f, line2);

    CHECK(line1.find("\"birth\"") != std::string::npos);
    CHECK(line1.find("\"tick\":10") != std::string::npos);
    CHECK(line2.find("\"death\"") != std::string::npos);
    CHECK(line2.find("\"starvation\"") != std::string::npos);

    std::filesystem::remove(tmp);
}

TEST_CASE("lineage log disabled with empty path") {
    gol::LineageLog log(std::filesystem::path{});
    CHECK_FALSE(log.enabled());
}
