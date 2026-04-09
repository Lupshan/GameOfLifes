#ifndef GAMEOFLIFES_ENGINE_SNAPSHOT_JSON_HPP
#define GAMEOFLIFES_ENGINE_SNAPSHOT_JSON_HPP

#include "engine/snapshot.hpp"
#include "engine/world.hpp"

#include <string>

namespace gol {

// Build a Snapshot from the current World state.
Snapshot capture_snapshot(const World& world);

// Serialize a Snapshot to a JSON string.
std::string snapshot_to_json(const Snapshot& snap);

// Deserialize a JSON string back to a Snapshot.
Snapshot snapshot_from_json(const std::string& json_str);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_SNAPSHOT_JSON_HPP
