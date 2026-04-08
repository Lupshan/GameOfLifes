# 12 — Bytecode spec

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Define the in-house bytecode format before writing either the VM or the compiler. This is a contract document — both sides depend on it. Locking it down up front avoids the "compiler emits one thing, VM expects another" trap.

## Scope
- `bytecode/spec/opcodes.md` — full opcode table: name, code, operands, stack effect, semantics.
- `bytecode/spec/format.md` — file format: magic header, version, constant pool, code section, metadata.
- Initial opcode set:
  - Stack: `PUSH_INT`, `PUSH_BOOL`, `POP`, `DUP`, `SWAP`.
  - Arithmetic: `ADD`, `SUB`, `MUL`, `DIV`, `MOD`, `NEG`.
  - Logic: `EQ`, `LT`, `GT`, `AND`, `OR`, `NOT`.
  - Control: `JMP`, `JMP_IF_FALSE`, `CALL`, `RETURN`.
  - Memory: `LOAD_LOCAL`, `STORE_LOCAL`.
  - Agent intrinsics (placeholders, finalized in step 14): `PERCEIVE`, `MOVE`, `EAT`, `REPRODUCE`, `TURN`.
- Semantics doc: instruction budget per tick, error behavior (stack underflow, division by zero), halt condition.

## Non-goals
- No implementation (steps 13–17).
- No language-level features (functions, closures) — those compile down to existing opcodes.

## Technical notes
- Bytecode is little-endian, fixed-width opcodes (1 byte) + variable operand width.
- Constant pool stores ints + strings (for debug names).
- ADR `008-bytecode-v1.md` records the design decision and trade-offs.

## Definition of Done
- [ ] `opcodes.md` + `format.md` + `semantics.md` written.
- [ ] ADR 008 written and merged.
- [ ] Spec is unambiguous enough that VM and compiler can be implemented independently from it.
- [ ] `docs/roadmap/README.md` table updated to `done`.
