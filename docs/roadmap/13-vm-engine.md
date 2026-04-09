# 13 — VM execution engine (C++)

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Implement the bytecode VM inside the engine. Pure execution — no agent binding yet (step 14). Given a bytecode blob, the VM runs instructions until it hits its budget, halts cleanly on errors, and is fully sandboxed (no allocations beyond its bounded stack).

## Scope
- `engine/include/engine/vm.hpp` — `Vm` class with `load(bytecode)`, `step()`, `run(instruction_budget)`.
- Bounded operand stack (e.g. 256 slots) and locals frame.
- Instruction decoder dispatching on opcode (giant switch or computed-goto if perf needs it later).
- Error model: returns `VmStatus { Ok, Halted, StackOverflow, BudgetExhausted, InvalidOpcode, ... }`. Never throws into the simulation loop.
- Loader for bytecode files (parses header + constant pool + code section per spec).
- Unit tests: each opcode tested in isolation, plus small programs (factorial, fibonacci, infinite loop hits budget).

## Non-goals
- No `PERCEIVE`/`MOVE`/etc. — those land in step 14 once the agent binding exists.
- No JIT, no optimization passes.
- No multi-threaded VM.

## Technical notes
- VM owns no heap allocation in the hot loop after `load`. Stack is `std::array`.
- All ops fit ≤20 lines; the dispatch switch is the one place this rule is relaxed (each case is tiny).
- Bytecode validation done at load time, not runtime.

## Definition of Done
- [x] All non-intrinsic opcodes from spec implemented + tested individually.
- [x] Sample programs (factorial, infinite loop) execute correctly.
- [x] Budget exhaustion halts cleanly.
- [x] No allocations in hot path (std::array stack + locals, manual review).
- [x] `docs/roadmap/README.md` table updated to `done`.
