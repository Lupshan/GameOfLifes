# ADR 010 — Bytecode format v1

**Status**: accepted
**Date**: 2026-04-09

## Context

The engine needs a way to execute user-submitted bot programs. Per ADR 002, we use a custom in-house bytecode rather than embedding an existing language runtime. This ADR records the design decisions for the v1 bytecode format.

## Decision

### Stack-based VM

A **stack-based** architecture rather than register-based. Trade-offs:

- **Pro**: simpler compiler (no register allocation), smaller instruction encoding (no register fields), easier to validate.
- **Con**: more instructions executed per operation (2 pushes + 1 op vs 1 op with 3 registers), harder to optimize.
- **Verdict**: simplicity wins for v1. The VM runs short programs (≤65KB) with a per-tick budget of ~1000 instructions. Register-based optimization is irrelevant at this scale.

### Fixed-width opcodes, variable operands

Opcodes are 1 byte (256 slots). Operand widths vary by instruction (0, 1, 2, or 4 bytes). This keeps common operations compact while allowing larger operands where needed.

### Instruction budget

Each agent gets a fixed number of instructions per tick (default: 1000). This is the primary sandboxing mechanism — it caps CPU time per agent and prevents infinite loops without needing to detect them.

Agent intrinsics cost 10× more than regular instructions, reflecting their real-world cost and preventing bots from spamming actions.

### Persistent locals, cleared stack

Local variables persist across ticks (agent "memory"). The stack is cleared each tick. This forces bots to use locals for state and keeps the stack from growing unbounded.

### No floating point

The VM operates exclusively on 32-bit signed integers. Floating-point adds complexity (NaN handling, platform-dependent rounding) for no clear benefit in a grid-based simulation. Bots can use fixed-point arithmetic if needed (multiply by 100, divide later).

### Binary format

The `.gol` file format uses a magic header, version field, constant pool, and code section. The format is versioned — the VM rejects unknown versions. Debug info is optional and stripped in production.

## Consequences

- The compiler (Python) targets a simple, well-documented instruction set.
- The VM (C++) implements a straightforward fetch-decode-execute loop.
- The 1000-instruction budget per tick makes the simulation predictable and fair.
- The lack of floating-point means some "natural" computations (angles, distances) are awkward. Acceptable — the game operates on a discrete grid.
- The v1 opcode set (46 opcodes) leaves room for ~200 future opcodes without bumping the version.
