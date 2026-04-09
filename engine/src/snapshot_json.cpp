#include "engine/snapshot_json.hpp"

#include <nlohmann/json.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace gol {

// ---------------------------------------------------------------------------
// Base64 encode / decode (RFC 4648, no padding variant not used — we pad).
// ---------------------------------------------------------------------------

namespace {

const std::array<char, 64> BASE64_CHARS = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

int base64_index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+') {
        return 62;
    }
    if (c == '/') {
        return 63;
    }
    return -1;
}

std::string base64_encode(const std::vector<std::uint8_t>& bytes) {
    std::string out;
    out.reserve(((bytes.size() + 2) / 3) * 4);

    std::size_t i = 0;
    while (i + 2 < bytes.size()) {
        std::uint32_t triple = (static_cast<std::uint32_t>(bytes[i]) << 16) |
                               (static_cast<std::uint32_t>(bytes[i + 1]) << 8) |
                               static_cast<std::uint32_t>(bytes[i + 2]);
        out.push_back(BASE64_CHARS[(triple >> 18) & 0x3F]);
        out.push_back(BASE64_CHARS[(triple >> 12) & 0x3F]);
        out.push_back(BASE64_CHARS[(triple >> 6) & 0x3F]);
        out.push_back(BASE64_CHARS[triple & 0x3F]);
        i += 3;
    }

    if (i + 1 == bytes.size()) {
        std::uint32_t val = static_cast<std::uint32_t>(bytes[i]) << 16;
        out.push_back(BASE64_CHARS[(val >> 18) & 0x3F]);
        out.push_back(BASE64_CHARS[(val >> 12) & 0x3F]);
        out.push_back('=');
        out.push_back('=');
    } else if (i + 2 == bytes.size()) {
        std::uint32_t val = (static_cast<std::uint32_t>(bytes[i]) << 16) |
                            (static_cast<std::uint32_t>(bytes[i + 1]) << 8);
        out.push_back(BASE64_CHARS[(val >> 18) & 0x3F]);
        out.push_back(BASE64_CHARS[(val >> 12) & 0x3F]);
        out.push_back(BASE64_CHARS[(val >> 6) & 0x3F]);
        out.push_back('=');
    }

    return out;
}

std::vector<std::uint8_t> base64_decode(const std::string& encoded) {
    std::vector<std::uint8_t> out;
    out.reserve((encoded.size() / 4) * 3);

    std::size_t i = 0;
    while (i < encoded.size()) {
        if (encoded[i] == '=') {
            break;
        }

        int a = base64_index(encoded[i]);
        int b = (i + 1 < encoded.size()) ? base64_index(encoded[i + 1]) : -1;
        int c = (i + 2 < encoded.size()) ? base64_index(encoded[i + 2]) : -1;
        int d = (i + 3 < encoded.size()) ? base64_index(encoded[i + 3]) : -1;

        if (a < 0 || b < 0) {
            throw std::runtime_error("snapshot_from_json: invalid base64 character");
        }

        std::uint32_t triple = (static_cast<std::uint32_t>(a) << 18) |
                               (static_cast<std::uint32_t>(b) << 12);
        out.push_back(static_cast<std::uint8_t>((triple >> 16) & 0xFF));

        if (c >= 0) {
            triple |= (static_cast<std::uint32_t>(c) << 6);
            out.push_back(static_cast<std::uint8_t>((triple >> 8) & 0xFF));
        }
        if (d >= 0) {
            triple |= static_cast<std::uint32_t>(d);
            out.push_back(static_cast<std::uint8_t>(triple & 0xFF));
        }

        i += 4;
    }

    return out;
}

// Pack a vector of 0/1 uint8_t values into a bitfield (MSB first within each byte).
std::vector<std::uint8_t> pack_bits(const std::vector<std::uint8_t>& cells) {
    std::size_t n_bytes = (cells.size() + 7) / 8;
    std::vector<std::uint8_t> packed(n_bytes, 0);
    for (std::size_t i = 0; i < cells.size(); ++i) {
        if (cells[i] != 0) {
            packed[i / 8] |= static_cast<std::uint8_t>(1 << (7 - (i % 8)));
        }
    }
    return packed;
}

// Unpack a bitfield back to 0/1 uint8_t values.
std::vector<std::uint8_t> unpack_bits(const std::vector<std::uint8_t>& packed,
                                      std::size_t total_cells) {
    std::vector<std::uint8_t> cells(total_cells, 0);
    for (std::size_t i = 0; i < total_cells; ++i) {
        if (i / 8 < packed.size()) {
            bool set = (packed[i / 8] & (1 << (7 - (i % 8)))) != 0;
            cells[i] = set ? 1 : 0;
        }
    }
    return cells;
}

std::string genome_to_hex(const Genome& g) {
    std::ostringstream oss;
    for (auto byte : g) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

Genome hex_to_genome(const std::string& hex) {
    Genome g{};
    for (std::size_t i = 0; i < GENOME_LENGTH && i * 2 + 1 < hex.size(); ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        g[i] = static_cast<std::uint8_t>(std::stoul(byte_str, nullptr, 16));
    }
    return g;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Capture
// ---------------------------------------------------------------------------

Snapshot capture_snapshot(const World& world) {
    Snapshot snap;
    snap.tick = world.tick_count();
    snap.width = world.config().width;
    snap.height = world.config().height;
    snap.seed = world.config().seed;

    for (const auto& agent : world.agents()) {
        snap.agents.push_back(SnapshotAgent{agent.id, agent.pos, agent.energy, agent.alive,
                                            agent.genome, agent.parent_id, agent.generation});
    }

    const auto& food_grid = world.food();
    std::size_t total = static_cast<std::size_t>(snap.width) * static_cast<std::size_t>(snap.height);
    snap.food.resize(total);
    for (int y = 0; y < snap.height; ++y) {
        for (int x = 0; x < snap.width; ++x) {
            std::size_t idx =
                static_cast<std::size_t>(y) * static_cast<std::size_t>(snap.width) +
                static_cast<std::size_t>(x);
            snap.food[idx] = food_grid.at(Position{x, y});
        }
    }

    return snap;
}

// ---------------------------------------------------------------------------
// JSON serialization
// ---------------------------------------------------------------------------

std::string snapshot_to_json(const Snapshot& snap) {
    nlohmann::json j;
    j["version"] = snap.version;
    j["tick"] = snap.tick;
    j["width"] = snap.width;
    j["height"] = snap.height;
    j["seed"] = snap.seed;

    nlohmann::json agents_arr = nlohmann::json::array();
    for (const auto& a : snap.agents) {
        nlohmann::json aj;
        aj["id"] = a.id;
        aj["x"] = a.pos.x;
        aj["y"] = a.pos.y;
        aj["energy"] = a.energy;
        aj["alive"] = a.alive;
        aj["genome"] = genome_to_hex(a.genome);
        aj["parent_id"] = a.parent_id;
        aj["generation"] = a.generation;
        agents_arr.push_back(aj);
    }
    j["agents"] = agents_arr;

    // Food: pack bits then base64 encode.
    std::vector<std::uint8_t> packed = pack_bits(snap.food);
    j["food"] = base64_encode(packed);

    return j.dump(2);
}

Snapshot snapshot_from_json(const std::string& json_str) {
    nlohmann::json j = nlohmann::json::parse(json_str);

    Snapshot snap;
    snap.version = j.at("version").get<int>();
    snap.tick = j.at("tick").get<std::uint64_t>();
    snap.width = j.at("width").get<int>();
    snap.height = j.at("height").get<int>();
    snap.seed = j.at("seed").get<std::uint64_t>();

    for (const auto& aj : j.at("agents")) {
        SnapshotAgent a;
        a.id = aj.at("id").get<std::uint64_t>();
        a.pos.x = aj.at("x").get<int>();
        a.pos.y = aj.at("y").get<int>();
        a.energy = aj.at("energy").get<int>();
        a.alive = aj.at("alive").get<bool>();
        if (aj.contains("genome")) {
            a.genome = hex_to_genome(aj.at("genome").get<std::string>());
        }
        if (aj.contains("parent_id")) {
            a.parent_id = aj.at("parent_id").get<std::uint64_t>();
        }
        if (aj.contains("generation")) {
            a.generation = aj.at("generation").get<std::uint64_t>();
        }
        snap.agents.push_back(a);
    }

    std::string food_b64 = j.at("food").get<std::string>();
    std::vector<std::uint8_t> packed = base64_decode(food_b64);
    std::size_t total = static_cast<std::size_t>(snap.width) * static_cast<std::size_t>(snap.height);
    snap.food = unpack_bits(packed, total);

    return snap;
}

} // namespace gol
