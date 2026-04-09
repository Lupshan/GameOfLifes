# ADR 011 — GoL Script language v1

**Status**: accepted
**Date**: 2026-04-09

## Context

Users need a way to write bot programs without hand-assembling bytecode. The language must be simple enough for beginners (the target includes students and non-engineers), compile to the existing bytecode opcode set, and not introduce runtime dependencies.

## Decision

### Language style: C-like, brace-based

Chose C-like syntax (`if`, `while`, braces, semicolons) over:
- **Python-style** (indentation-based): harder to parse, whitespace sensitivity is a source of bugs for beginners in a web editor context.
- **Lisp-style**: powerful but unfamiliar to most users, parentheses-heavy.
- **Custom visual** (Blockly only): Blockly is already planned (step 29) but a text language is needed for advanced users.

### One numeric type: `int`

No floats, no strings. The simulation operates on a discrete integer grid. This eliminates an entire class of type errors and keeps the compiler trivial.

Booleans are ints (0/nonzero). No separate bool type in the type system — just in the syntax (`true`/`false` literals).

### No block scoping

Variables are function-scoped, not block-scoped. This simplifies compilation to flat local variable slots. Block scoping can be added in v2 if users request it.

### Persistent locals

Local variables persist across ticks (they map to VM locals which survive `reset_for_tick`). This gives agents implicit memory without needing a separate "memory" API. Trade-off: uninitialized variables silently start at 0 rather than causing an error.

### Perception convenience functions

`perceive()` and `look()` push 4 values, which is awkward in a language with no tuples. Rather than adding tuples (complex), we provide `perceive_food()`, `perceive_terrain()`, etc. as compiler sugar that emits PERCEIVE + stack manipulation to extract one value.

## Consequences

- The compiler is a straightforward single-pass recursive descent parser + codegen.
- No type checker needed (everything is int).
- The language is restrictive but sufficient for interesting bot behaviors (food seeking, reproduction strategies, terrain avoidance).
- Advanced users who want more expressiveness can hand-write bytecode or request language extensions (tracked as v2 features).
- The Blockly editor (step 29) will generate GoL Script, which is then compiled to bytecode — two-stage pipeline.
