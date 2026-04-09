# Bytecode execution semantics — v1

## Execution model

Each agent runs a **single bytecode program**. The VM executes the program once per tick, starting from the code section entry point (byte 0). Execution continues until:

1. A `HALT` instruction is reached.
2. The **instruction budget** for the tick is exhausted.
3. A **runtime error** occurs.

At the start of each tick, the VM resets the instruction pointer to 0 but **preserves local variables** across ticks. This allows agents to maintain state between ticks.

## Instruction budget

Each tick, an agent has a fixed number of instructions it can execute. This prevents infinite loops and ensures fair scheduling.

- **Default budget**: 1000 instructions per tick.
- Budget is configurable via `WorldConfig` (`instruction_budget`).
- Every opcode costs exactly **1 instruction** from the budget, except:
  - `NOP`: costs 1 (no free lunch).
  - Agent intrinsics (`MOVE`, `EAT`, etc.): cost **10 instructions** each (they represent costly real-world actions).
- When budget reaches 0, execution stops immediately. The agent resumes at byte 0 next tick.

## Stack

- The operand stack is a LIFO stack of 32-bit signed integers.
- Booleans are represented as integers: 0 = false, nonzero = true.
- Maximum stack depth: **256 entries**. Exceeding this is a runtime error.
- The stack is **cleared** at the start of each tick.

## Local variables

- Up to **255** local variable slots (indexed 0–254).
- Each slot holds a 32-bit signed integer, initialized to 0 at program start.
- Locals **persist across ticks** — this is the agent's "memory."
- `LOAD_LOCAL` and `STORE_LOCAL` access locals by index.

## Call frames

- `CALL addr` pushes a call frame (return address + local frame) and jumps to `addr`.
- `RETURN` pops the call frame and resumes at the return address.
- Maximum call depth: **32 frames**. Exceeding this is a runtime error.
- Calls share the same local variable space (no per-frame locals in v1).

## Runtime errors

When a runtime error occurs:

1. The agent's execution for the current tick **stops immediately**.
2. The error is logged (lineage log, `"type": "vm_error"`).
3. The agent **does not die** from a VM error — it simply does nothing for the rest of the tick.
4. Next tick, execution restarts from byte 0.

Runtime error conditions:
- **Stack underflow**: pop from empty stack.
- **Stack overflow**: push beyond 256 entries.
- **Division by zero**: `DIV` or `MOD` with zero divisor.
- **Invalid opcode**: byte doesn't correspond to any known opcode.
- **Call depth exceeded**: `CALL` with 32 frames already on the call stack.
- **Out-of-bounds access**: `LOAD_LOCAL` / `STORE_LOCAL` with index >= `local_count`.
- **Out-of-bounds jump**: `JMP` / `JMP_IF_FALSE` target outside code section.

## Agent intrinsics semantics

Agent intrinsics interact with the world. They can fail (e.g., `EAT` on an empty cell), in which case they push a result indicating failure but do not cause a runtime error.

### PERCEIVE (0x80)
- Cost: 10 instructions.
- Pushes 4 values (bottom to top): terrain type (int), food count, water count, number of agents on current cell.
- Always succeeds (passive perception is free per ADR 005).

### LOOK (0x81)
- Cost: 10 instructions.
- Pushes 4 values for the cell directly ahead of the agent.
- Vision may be reduced at night (values clamped or zeroed beyond effective range).

### MOVE (0x82)
- Cost: 10 instructions.
- Moves agent forward one cell in its facing direction. Terrain cost is deducted from energy.
- Always succeeds (toroidal world, no blocking).

### TURN_LEFT (0x83), TURN_RIGHT (0x84)
- Cost: 10 instructions each.
- Rotate agent's facing direction 90°.

### EAT (0x85)
- Cost: 10 instructions.
- Consumes food on current cell. Pushes energy gained (0 if no food).

### DRINK (0x86)
- Cost: 10 instructions.
- Consumes water on current cell. Pushes hydration gained (0 if no water).

### REPRODUCE (0x87)
- Cost: 10 instructions.
- Attempts asexual reproduction. Pushes 1 if successful, 0 if energy < threshold.
- On success: child spawned adjacent, parent loses half energy.

### MY_ENERGY (0x88), MY_HYDRATION (0x89)
- Cost: 1 instruction each (these are reads, not actions).
- Push the agent's current energy / hydration.

### MY_TRAIT (0x8A)
- Cost: 1 instruction.
- Pops nothing, reads inline `trait_id` operand.
- Pushes the decoded value of the specified trait.
- trait_id mapping: 0=vision, 1=speed, 2=metabolism, 3=reproduction_threshold, 4=swim, 5=forage_efficiency.

## Halting

- `HALT` (0x00) ends execution for the tick. Remaining budget is discarded.
- If the code section is exhausted without a `HALT`, execution stops (implicit halt at end of code).

## Determinism

All bytecode execution is **deterministic** given the same world state and RNG state. The VM does not use floating-point arithmetic internally. Agent intrinsics that involve randomness (e.g., `REPRODUCE` child placement) use the world RNG.
