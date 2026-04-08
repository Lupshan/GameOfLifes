# 04 — World grid + primitives

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Introduce the spatial primitives every later step depends on: `Position`, `Direction`, and a toroidal `Grid<T>` storage type. Pure data, no agents, no simulation — but tested thoroughly because every bug here will haunt every later step.

## Scope
- `engine/include/engine/position.hpp` — `Position { int x; int y; }` with equality, hash.
- `engine/include/engine/direction.hpp` — `enum class Direction { North, East, South, West }` with `to_offset()` and `turn_left/right`.
- `engine/include/engine/grid.hpp` — templated `Grid<T>` with `width`, `height`, `at(Position)` (toroidal wrap), `set(Position, T)`, iterator support.
- Toroidal wrap helper `wrap(int v, int size)` handling negative coords.
- Unit tests: wrap (positive, negative, zero), `Grid::at` access patterns, direction turning.

## Non-goals
- No `World` aggregate yet (that's step 06 once agents arrive).
- No serialization (step 07).
- No multi-layer grids (step 10 introduces them).

## Technical notes
- `Grid<T>` stores a flat `std::vector<T>` of size `width * height`. Index = `y * width + x` (after wrap).
- Wrap formula: `((v % size) + size) % size`.
- Templated, so header-only is acceptable here (genuine template requirement — exception to the "no impl in header" rule).
- `Position` `operator==` and `std::hash` specialization in the same header.

## Definition of Done
- [x] All headers exist with proper guards (`GAMEOFLIFES_ENGINE_*_HPP`).
- [x] `Grid<int>` instantiable; round-trips set/at correctly.
- [x] Wrap unit tests cover: positive in-bounds, positive overflow, negative, zero size guarded.
- [x] Direction unit tests cover all 4 turns + offsets.
- [x] No warnings.
- [x] `docs/roadmap/README.md` table updated to `done`.

> Note : divergence ADR 005/006 (monde non toroïdal, chunké) vs roadmap (toroïdal). Voir `docs/roadmap/skipped.md` — option (a) retenue, à reconfirmer avant step 06.
