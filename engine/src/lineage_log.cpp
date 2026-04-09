#include "engine/lineage_log.hpp"

#include <nlohmann/json.hpp>

#include <iomanip>
#include <sstream>

namespace gol {

namespace {

std::string genome_to_hex(const Genome& g) {
    std::ostringstream oss;
    for (auto byte : g) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

} // namespace

LineageLog::LineageLog(const std::filesystem::path& path) : enabled_(!path.empty()) {
    if (enabled_) {
        file_.open(path, std::ios::out | std::ios::trunc);
        if (!file_) {
            enabled_ = false;
        }
    }
}

bool LineageLog::enabled() const noexcept {
    return enabled_;
}

void LineageLog::log_birth(std::uint64_t tick, std::uint64_t id, std::uint64_t parent_id,
                           std::uint64_t generation, const Genome& genome) {
    if (!enabled_) {
        return;
    }
    nlohmann::json j;
    j["tick"] = tick;
    j["type"] = "birth";
    j["id"] = id;
    j["parent"] = parent_id;
    j["gen"] = generation;
    j["genome"] = genome_to_hex(genome);
    file_ << j.dump() << "\n";
}

void LineageLog::log_death(std::uint64_t tick, std::uint64_t id, const std::string& cause) {
    if (!enabled_) {
        return;
    }
    nlohmann::json j;
    j["tick"] = tick;
    j["type"] = "death";
    j["id"] = id;
    j["cause"] = cause;
    file_ << j.dump() << "\n";
}

} // namespace gol
