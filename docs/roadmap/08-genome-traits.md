# 08 — Genome, traits, mutation

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Introduce heritable variation. Agents now have a `Genome` decoding into trait values that influence simulation behavior (vision, speed, metabolism…). This is the foundation of the project's central pillar — evolution.

## Scope
- `engine/include/engine/genome.hpp` — `Genome` as a fixed-length `std::array<std::uint8_t, N>`.
- `engine/include/engine/traits.hpp` — `Traits { int vision; int speed; int metabolism; int reproduction_threshold; ... }` decoded from a genome.
- `decode(const Genome&) -> Traits` deterministic mapping (each gene byte → one trait, with clamping).
- `mutate(const Genome&, Rng&, double rate) -> Genome` flips bits with probability `rate`.
- `Agent` gains a `Genome` field.
- Tick loop uses `traits.metabolism` for energy loss, `traits.speed` for moves/tick.
- Unit tests: decode is deterministic, mutate respects rate (statistical), bit flips actually happen.

## Non-goals
- No reproduction yet (step 09).
- No phenotypic plasticity, no epigenetics.
- No "neural genome" — pure parametric.

## Technical notes
- Genome length: start at 16 bytes, document in ADR `006-genome-format.md`.
- Trait clamping: each trait has min/max in `traits.hpp` constants.
- Mutation rate is a `WorldConfig` parameter.

## Definition of Done
- [x] `Genome`, `Traits`, `decode`, `mutate` exist with tests.
- [x] `Agent` carries a genome; tick uses decoded traits.
- [x] Statistical mutation test (≥1000 trials) within tolerance.
- [x] ADR 008 written (006 was already taken by world-generation).
- [x] `docs/roadmap/README.md` table updated to `done`.
