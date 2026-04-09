# ADR 008 — Genome format

**Status**: accepted
**Date**: 2026-04-09

## Context

Step 08 introduces heritable variation via a genome. We need a format that is simple to implement, fast to decode, and extensible for future traits without breaking existing genomes.

## Decision

### Genome structure

A genome is a **fixed-length array of 16 bytes** (`std::array<std::uint8_t, 16>`).

Each byte maps to one trait via a deterministic `decode()` function. The mapping is positional:

| Byte index | Trait                    | Range        |
|------------|--------------------------|--------------|
| 0          | vision                   | 1–5          |
| 1          | speed (moves/tick)       | 1–3          |
| 2          | metabolism (energy/tick)  | 1–5          |
| 3          | reproduction threshold   | 50–200       |
| 4–15       | reserved for future use  | —            |

### Byte-to-trait mapping

Linear interpolation: `trait = lo + round(byte / 255.0 * (hi - lo))`. Byte 0 maps to `lo`, byte 255 maps to `hi`. Deterministic, no floating-point surprises.

### Mutation

Bit-level mutation: each of the 128 bits is independently flipped with probability `mutation_rate` (a `WorldConfig` parameter, default 0.01). This gives fine-grained variation — a single bit flip in the metabolism byte might change the trait value by 1, or might not cross a threshold at all.

### Serialization

In snapshot JSON, genome is stored as a **32-character hex string** (2 hex chars per byte). Human-readable and compact.

### Extension policy

New traits claim the next unused byte index. The 16-byte length provides room for 12 more traits. If 16 bytes is insufficient, a new ADR bumps the length and the snapshot schema version.

## Consequences

- Every agent now carries 16 bytes of heritable state.
- The simulation tick uses decoded traits for energy drain (metabolism) and movement count (speed).
- Vision and reproduction_threshold are decoded but not yet used — they become active in steps 09 and beyond.
- Mutation operates at the bit level, giving smoother evolutionary pressure than byte-level mutation.
- The fixed-length format means no allocation overhead per agent.
