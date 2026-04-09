#ifndef GAMEOFLIFES_ENGINE_LINEAGE_LOG_HPP
#define GAMEOFLIFES_ENGINE_LINEAGE_LOG_HPP

#include "engine/genome.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace gol {

class LineageLog {
  public:
    // Open a lineage log file for writing. Pass empty path to disable logging.
    explicit LineageLog(const std::filesystem::path& path);

    bool enabled() const noexcept;

    void log_birth(std::uint64_t tick, std::uint64_t id, std::uint64_t parent_id,
                   std::uint64_t generation, const Genome& genome);

    void log_death(std::uint64_t tick, std::uint64_t id, const std::string& cause);

  private:
    std::ofstream file_;
    bool enabled_;
};

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_LINEAGE_LOG_HPP
