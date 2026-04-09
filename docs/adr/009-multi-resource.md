# ADR 009 — Multi-resource ecosystem and snapshot v2

**Status**: accepted
**Date**: 2026-04-09

## Context

A single food resource creates a trivial optimization landscape — one optimal genome wins and the ecosystem converges. To create real ecological niches and emergent diversity, the world needs multiple resources and terrain types that create trade-offs.

## Decision

### Resources

Three resource types, each stored as a separate `Grid<uint8_t>`:

| Resource | Purpose | Regen rate | Key terrain |
|----------|---------|------------|-------------|
| Food     | Energy  | Base × terrain factor | Forest (3×) |
| Water    | Hydration | Base × 0.5 × terrain factor | Water terrain (5×) |
| Mineral  | Reproduction | Base × 0.1 × terrain factor | Rock (3×) |

Separate grids (struct-of-arrays) rather than a single per-cell struct: the hot loop iterates one resource at a time during regeneration and agent foraging, so this layout is cache-friendly.

### Terrain

`enum class Terrain : uint8_t { Plain, Forest, Water, Rock }`. Stored in `Grid<Terrain>`.

Terrain affects:
- **Movement cost**: Water = 3, Rock = 2, others = 1. The `swim` trait reduces water cost.
- **Resource regeneration**: each terrain has per-resource multipliers (e.g. forest grows food 3× faster, rock has 3× more minerals).

### Agent changes

- `hydration` field, drains 1/tick. Death at ≤ 0.
- Agents drink water resources on contact.

### New traits (genome bytes 4-5)

| Byte | Trait | Range | Effect |
|------|-------|-------|--------|
| 4 | swim | 0–3 | Reduces water terrain movement cost |
| 5 | forage_efficiency | 1–3 | Multiplier for resource pickup |

### Snapshot v2

Version bumped to 2. New fields: `water`, `mineral` (base64-packed bits like food), `terrain` (base64-encoded raw bytes), agent `hydration`.

## Consequences

- Ecological niches now possible: forest-dwellers (high forage, low metabolism), water-adapted swimmers, rock-miners for reproduction.
- Trait trade-offs become meaningful: a fast agent with high metabolism needs more food, which is terrain-dependent.
- The selection pressure test shows these dynamics in action over 1000+ ticks.
- Snapshot v2 is backward-compatible for reading (new fields have `contains()` guards).
