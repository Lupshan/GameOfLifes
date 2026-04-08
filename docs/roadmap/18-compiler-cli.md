# 18 — Compiler CLI + round-trip tests

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Wrap the compiler in a CLI tool and prove the full pipeline (source → bytecode → engine VM execution → expected world state) works end to end with an automated test suite spanning both Python and C++.

## Scope
- `bytecode/compiler/__main__.py` — `python -m bytecode.compiler <input.gol> -o <output.gob>`.
- `just compile <file>` recipe.
- Round-trip integration test: a Python test compiles `random_walker.gol`, invokes the C++ engine binary as a subprocess with the produced `.gob`, parses snapshots, asserts agent population behaves like the hand-written bytecode baseline (step 14).
- CI runs the round-trip test in a dedicated job.

## Non-goals
- No language server / LSP.
- No nice error formatting (just file:line:col + message).

## Technical notes
- Cross-brick test lives under `bytecode/tests/integration/` and depends on the engine binary being built — CI orders the engine job before this test.
- Engine binary path is found via `cmake --build` output or env var `GAMEOFLIFES_ENGINE_BIN`.

## Definition of Done
- [ ] Compiler CLI works.
- [ ] Round-trip integration test passes locally.
- [ ] CI runs it and is green.
- [ ] `docs/roadmap/README.md` table updated to `done`.
