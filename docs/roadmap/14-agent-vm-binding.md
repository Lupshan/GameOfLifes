# 14 — Agent ↔ VM binding

**Status**: not started
**Started**: —
**Finished**: —

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
- [ ] All intrinsics implemented + tested.
- [ ] Hand-written `random_walker.gob` runs.
- [ ] Functional parity test with step 06 baseline passes.
- [ ] `docs/roadmap/README.md` table updated to `done`.
