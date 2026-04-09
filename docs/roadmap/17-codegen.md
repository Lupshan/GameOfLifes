# 17 — AST → bytecode codegen

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
The back half of the compiler: AST → bytecode blob in the spec's binary format. Plus a basic semantic check pass (undefined variables, arity mismatches).

## Scope
- `bytecode/compiler/sema.py` — symbol table, scope resolution, arity checks.
- `bytecode/compiler/codegen.py` — visitor producing a list of opcodes + constant pool.
- `bytecode/compiler/emit.py` — serializes to the binary `.gob` format per spec.
- Local variable allocation, label patching for jumps, function call frames.
- pytest tests: each AST node type emits expected opcode sequences; sample programs produce a `.gob` that the C++ VM accepts.

## Non-goals
- No optimization (constant folding, dead code elim) — deferred.
- No debug info beyond function names in the constant pool.

## Technical notes
- Two-pass codegen: first pass emits with placeholder jump offsets, second pass patches.
- Builtin function calls compile to the corresponding intrinsic opcode directly, not a `CALL`.

## Definition of Done
- [x] sema + codegen + emit implemented.
- [x] All sample programs compile to valid `.gob` files (22 codegen tests).
- [ ] Cross-brick smoke test deferred to step 18 (compiler CLI).
- [x] `docs/roadmap/README.md` table updated to `done`.
