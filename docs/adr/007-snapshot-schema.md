# ADR 007 — Snapshot JSON schema

**Status**: accepted
**Date**: 2026-04-09

## Context

The engine needs to expose world state to the outside (backend, frontend, replay storage). A serialization format is required that is human-readable, easy to produce and consume, and stable enough to serve as a contract between engine and backend.

## Decision

**JSON snapshots**, one per tick, with the following schema (version 1):

```json
{
  "version": 1,
  "tick": 42,
  "width": 64,
  "height": 64,
  "seed": 42,
  "agents": [
    { "id": 1, "x": 10, "y": 20, "energy": 95, "alive": true }
  ],
  "food": "<base64>"
}
```

### Food encoding

The food grid (`width * height` cells, each 0 or 1) is packed into a bitfield (MSB-first within each byte, row-major order) and then base64-encoded (RFC 4648 with padding). This keeps the JSON compact — a 64x64 grid is 682 base64 characters instead of 4096 raw values.

### Versioning

The top-level `"version"` field is an integer, starting at 1. Any breaking change to the schema requires a version bump and a new ADR. Consumers must check the version before parsing.

### Library

`nlohmann/json` v3.11.3 — community-driven, header-only, widely used, MIT license. No GAFAM-backed alternative needed; this is the de facto standard. Pinned via FetchContent.

## Consequences

- Engine can dump one JSON file per tick via `--dump-snapshots <dir>`.
- Backend and frontend can parse snapshots with any JSON library.
- Round-trip (serialize → deserialize) is lossless — tested.
- Binary or compressed formats can be introduced later (new version) if JSON becomes a bottleneck.
- `nlohmann/json` adds to build time on first fetch but is header-only with zero runtime deps.
