# 15 — Language grammar spec

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Design the user-facing scripting language before writing the compiler. Like step 12 for bytecode, this is a contract document. The language should be minimal, learnable in 10 minutes, and compile cleanly down to the existing opcode set.

## Scope
- `bytecode/spec/language.md` — full grammar in EBNF, type system, statement/expression list, builtin functions (mapping to intrinsics).
- Core features:
  - Integer + boolean types only (v1).
  - Local variables, `if/else`, `while`, function definitions, `return`.
  - Builtin functions: `move(dir)`, `perceive_food()`, `eat()`, `energy()`, `reproduce()`.
- Sample programs: random walker, food seeker, opportunistic eater.
- Document deliberate non-features: no strings, no floats, no closures, no FFI.

## Non-goals
- No implementation (next steps).
- No type inference beyond local — explicit types not required since only one numeric type exists.

## Technical notes
- Syntax style: brace-based, C-like, semicolon-terminated. Easier for users coming from Python/JS than ML-style.
- Each language feature must map to existing opcodes; if it doesn't, either drop it or extend the spec (with ADR).
- ADR `009-language-v1.md` records design decisions.

## Definition of Done
- [x] `language.md` complete with EBNF grammar + 4 sample programs.
- [x] Each sample is compilable to existing opcodes (compilation mapping table provided).
- [x] ADR 011 written (009 was taken by multi-resource).
- [x] `docs/roadmap/README.md` table updated to `done`.
