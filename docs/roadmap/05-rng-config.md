# 05 — RNG + config

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Make the simulation deterministic and configurable. Every random draw must come from a single seeded RNG owned by the simulation, and every tweakable parameter must live in a `WorldConfig` struct loaded from a config file.

## Scope
- `engine/include/engine/rng.hpp` — thin wrapper around `std::mt19937_64` with helpers `uniform_int(min, max)`, `uniform_real()`, `chance(p)`.
- `engine/include/engine/world_config.hpp` — POD struct with: `int width`, `int height`, `std::uint64_t seed`, `int initial_agents`, `int initial_food`, `int max_ticks` (0 = unbounded), `double food_spawn_rate`, `int starting_energy`.
- TOML config loader: `engine/src/world_config_loader.cpp` reads `engine/config/default.toml` into a `WorldConfig`.
- Dep: `tomlplusplus` (header-only, community-driven).
- Unit tests: same seed → same RNG sequence; config file round-trips.

## Non-goals
- No CLI flag parsing yet (deferred until executable grows).
- No per-agent RNG; one global per `World`.

## Technical notes
- RNG is **not** a global. Owned by `World` (added step 06). Tests construct their own.
- Determinism contract: any code path that draws randomness must take an `Rng&` parameter — never call `std::rand()`, never use thread-local state.
- `tomlplusplus` fetched via FetchContent, pinned.

## Definition of Done
- [ ] `Rng` wrapper exists with the listed helpers + tests.
- [ ] `WorldConfig` + loader works on `engine/config/default.toml`.
- [ ] Same seed → identical sequence verified by test.
- [ ] `tomlplusplus` pinned in CMake.
- [ ] `docs/roadmap/README.md` table updated to `done`.
