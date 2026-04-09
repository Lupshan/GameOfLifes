# 06 — Agents + tick loop

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
First living simulation. Combine grid + RNG + config into a `World` containing agents that wander, eat food, lose energy, and die. This is the smallest interesting system: it proves the core loop works end-to-end and gives every later step (genetics, VM, snapshots) something concrete to plug into.

## Scope
- `engine/include/engine/agent.hpp` — `Agent { Position pos; int energy; bool alive; std::uint64_t id; }`.
- `engine/include/engine/world.hpp` — `World` aggregating: `Grid<std::uint8_t>` (food layer), `std::vector<Agent>`, `Rng`, `WorldConfig`.
- `engine/include/engine/simulation.hpp` — `Simulation` driving the tick loop.
- Per-tick logic: each alive agent picks a random 4-neighbour, moves there (toroidal), eats food if present (+E energy, food cell cleared), loses 1 energy, dies if energy ≤ 0; food respawns randomly; dead agents reaped at end of tick.
- Executable `gameoflifes_engine` runs N ticks from `default.toml` and prints per-tick stats.
- Unit tests: food place/consume, agent move toroidal, agent reap.
- Functional tests: 100-tick run with invariants (positions in bounds, alive ⇒ energy > 0); starvation test (no food → all dead within bound).

## Non-goals
- No genetics, no reproduction (step 08–09).
- No multi-resource (step 10).
- No bytecode-driven behavior — agents are hardcoded random walkers.

## Technical notes
- Reap = `std::erase_if(agents, [](const Agent& a){ return !a.alive; })` after tick.
- Each tick increments a `tick_count` on the world.
- Stats printer is a free function in `engine/src/stats.cpp` taking `const World&`.
- Functions ≤20 lines; tick loop body extracted into `step_agent`, `respawn_food`, `reap_dead` helpers.

## Definition of Done
- [x] `World`, `Agent`, `Simulation` headers + sources respect CLAUDE.md C++ rules.
- [x] `just run` runs N ticks and prints stats.
- [x] Unit tests: food, agent move, reap.
- [x] Functional test: 100 ticks, invariants hold throughout.
- [x] Functional test: starvation bounded.
- [x] No warnings, `just test-engine` green.
- [x] `docs/roadmap/README.md` table updated to `done`.
