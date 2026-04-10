#ifndef GAMEOFLIFES_ENGINE_IPC_HPP
#define GAMEOFLIFES_ENGINE_IPC_HPP

#include "engine/snapshot_json.hpp"
#include "engine/world.hpp"

#include <string>

namespace gol {

// Read a line from stdin without blocking (returns empty if nothing available).
// On EOF, returns "EOF".
std::string try_read_line();

// Emit a JSON event to stdout (line-delimited).
void emit_event(const std::string& json_line);

// Emit a ready event.
void emit_ready();

// Emit a snapshot event from current world state.
void emit_snapshot(const World& world);

// Emit an ack event.
void emit_ack(const std::string& cmd);

// Emit an error event.
void emit_error(const std::string& message);

// Run the IPC event loop: reads commands from stdin, runs ticks, emits snapshots.
int run_ipc_mode(const std::string& config_path);

} // namespace gol

#endif // GAMEOFLIFES_ENGINE_IPC_HPP
