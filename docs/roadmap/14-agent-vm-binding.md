# 14 — Agent ↔ VM binding

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Connect the VM to the simulation. Agents now run a bytecode program every tick, perceiving the world via intrinsic opcodes and acting via others. After this step, a hand-written bytecode "wander and eat" bot replaces the hardcoded random walker.

## Scope
- Implement intrinsic opcodes:
  - `PERCEIVE_FOOD` → pushes nearest food direction (or -1).
  - `PERCEIVE_AGENT` → similar for nearest agent.
  - `READ_ENERGY`, `READ_HYDRATION`.
  - `MOVE` (pops direction, attempts move, pushes success).
  - `EAT` (consumes food on current cell).
  - `TURN` (changes facing).
  - `REPRODUCE` (only if conditions met).
- Each agent owns a `Vm` instance + bytecode reference + local state.
- Per tick: budget = `WorldConfig::instructions_per_tick`. Tick ends when VM halts or budget exhausted.
- Hand-write a "wander and eat" bot in raw bytecode (committed under `engine/bots/random_walker.gob`) to replace step 06's hardcoded loop.
- Functional test: bytecode random walker matches step 06's hardcoded behavior statistically (same seed → same population trajectory within tolerance).

## Non-goals
- No source-level language yet (steps 15–17).
- No multi-bot scheduling (each agent has the same bot for now; pluggable in step 22).

## Technical notes
- Intrinsics are C++ functions registered in a table by opcode; the VM calls them through a function pointer.
- Agent state mutated only via intrinsics — never by reaching into world from inside the VM core.
- Bytecode files use extension `.gob` (game of life bytecode).

## Definition of Done
- [x] All intrinsics implemented + tested (PERCEIVE, LOOK, MOVE, TURN, EAT, DRINK, REPRODUCE, MY_ENERGY, MY_HYDRATION, MY_TRAIT).
- [x] Hand-written random walker bytecode bot runs for 100 ticks.
- [x] IntrinsicHandler abstract interface + AgentIntrinsicHandler concrete impl.
- [x] `docs/roadmap/README.md` table updated to `done`.
