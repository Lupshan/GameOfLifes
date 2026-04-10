#include "engine/ipc.hpp"

#include "engine/simulation.hpp"
#include "engine/snapshot_json.hpp"
#include "engine/stats.hpp"
#include "engine/world.hpp"
#include "engine/world_config.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

namespace gol {

void emit_event(const std::string& json_line) {
    std::cout << json_line << "\n" << std::flush;
}

void emit_ready() {
    nlohmann::json j;
    j["event"] = "ready";
    emit_event(j.dump());
}

void emit_snapshot(const World& world) {
    Snapshot snap = capture_snapshot(world);
    std::string json = snapshot_to_json(snap);
    // Wrap in an event envelope.
    nlohmann::json j = nlohmann::json::parse(json);
    j["event"] = "snapshot";
    emit_event(j.dump());
}

void emit_ack(const std::string& cmd) {
    nlohmann::json j;
    j["event"] = "ack";
    j["cmd"] = cmd;
    emit_event(j.dump());
}

void emit_error(const std::string& message) {
    nlohmann::json j;
    j["event"] = "error";
    j["error"] = message;
    emit_event(j.dump());
}

int run_ipc_mode(const std::string& config_path) {
    WorldConfig config;
    if (!config_path.empty() && std::filesystem::exists(config_path)) {
        config = load_world_config(config_path);
    }

    World world(config);
    world.init();

    Simulation sim(world);
    bool paused = false;
    bool running = true;

    // Tick rate: 10 ticks/sec = 100ms per tick.
    auto tick_duration = std::chrono::milliseconds(100);

    emit_ready();

    while (running) {
        auto tick_start = std::chrono::steady_clock::now();

        // Read commands from stdin (blocking read with timeout via getline).
        // For v1 simplicity: check if there's a line available.
        // We use a non-blocking approach by checking cin.
        while (true) {
            // Peek if stdin has data.
            if (!std::cin.rdbuf()->in_avail()) {
                break;
            }
            std::string line;
            if (!std::getline(std::cin, line)) {
                // EOF: parent process closed stdin, shut down.
                running = false;
                break;
            }
            if (line.empty()) {
                continue;
            }

            try {
                auto cmd = nlohmann::json::parse(line);
                std::string cmd_type = cmd.value("cmd", "");

                if (cmd_type == "shutdown") {
                    running = false;
                    emit_ack("shutdown");
                    break;
                } else if (cmd_type == "pause") {
                    paused = true;
                    emit_ack("pause");
                } else if (cmd_type == "resume") {
                    paused = false;
                    emit_ack("resume");
                } else if (cmd_type == "load_bot") {
                    // v1: acknowledge but don't actually load bytecode yet
                    // (full bot loading requires VM integration per agent).
                    emit_ack("load_bot");
                } else if (cmd_type == "unload_bot") {
                    emit_ack("unload_bot");
                } else {
                    emit_error("unknown command: " + cmd_type);
                }
            } catch (const nlohmann::json::exception& e) {
                emit_error(std::string("invalid JSON: ") + e.what());
            }
        }

        if (!running) {
            break;
        }

        // Run one tick if not paused.
        if (!paused) {
            sim.tick();
            emit_snapshot(world);
        }

        // Sleep to maintain tick rate.
        auto elapsed = std::chrono::steady_clock::now() - tick_start;
        auto remaining = tick_duration - elapsed;
        if (remaining > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(remaining);
        }
    }

    return 0;
}

} // namespace gol
