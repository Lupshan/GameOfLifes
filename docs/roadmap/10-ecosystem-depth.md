# 10 — Multi-resource ecosystem

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Make the world less trivial: multiple resource types and terrain so that traits actually trade off against each other and niches can emerge. Without this step, evolution converges on a single optimal strategy and the simulation feels dead.

## Scope
- Resources: `food` (regrows slowly), `water` (sparse, refills agent's hydration), `mineral` (rare, used by reproduction).
- `Agent` gains `hydration` field; dies if 0.
- Terrain layer: `enum class Terrain { Plain, Forest, Water, Rock }` stored in a parallel `Grid<Terrain>`. Movement cost varies; some terrain blocks movement.
- Resource regeneration rules per terrain (forest grows food faster, etc.).
- New traits: `swim`, `forage_efficiency`.
- Functional test: a seeded world with two distinct biomes shows trait divergence between subpopulations.

## Non-goals
- No weather/climate yet (step 11).
- No player-modifiable terrain.
- No A* pathfinding — agents still take local decisions only.

## Technical notes
- World now holds a `Grid<Terrain>` + per-resource `Grid<std::uint8_t>` layers, all same dims.
- Snapshot schema bumped to v2 (ADR `007-multi-resource.md`).
- Document trade-offs in ADR: chose multiple parallel grids over a single struct-of-arrays cell type for cache locality on hot loops.

## Definition of Done
- [x] Multiple resource grids + terrain grid implemented (food, water, mineral).
- [x] Hydration mechanic + tests.
- [x] New traits decoded and used (swim, forage_efficiency).
- [x] Biome divergence functional test passes.
- [x] Snapshot v2 schema documented (ADR 009).
- [x] `docs/roadmap/README.md` table updated to `done`.
