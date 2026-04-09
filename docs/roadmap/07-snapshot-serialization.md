# 07 — World snapshot serialization

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Make the world state observable from outside the engine. Serialize a tick to JSON and write it to stdout/file. Every later step that involves the backend, frontend, or replays will consume this format.

## Scope
- `engine/include/engine/snapshot.hpp` — `Snapshot` struct mirroring `World` (tick number, dimensions, agents list, food layer encoded compactly).
- `engine/src/snapshot_json.cpp` — `to_json(const World&) -> std::string` and inverse `from_json`.
- Dep: `nlohmann/json` (community-driven, header-only).
- CLI flag `--dump-snapshots <dir>` on `gameoflifes_engine` writing one `tick_NNNN.json` per tick.
- Round-trip tests: serialize → deserialize → equal.

## Non-goals
- No binary format (JSON is fine until it isn't).
- No streaming protocol yet (step 24 introduces WebSocket framing).
- No compression.

## Technical notes
- Food layer encoded as RLE or base64-packed bits (decide at impl time, document choice).
- Snapshot schema versioned: top-level `"version": 1` field. Bumps require ADR.
- JSON is the wire format between engine and backend → schema is a contract.
- `nlohmann/json` pinned via FetchContent.

## Definition of Done
- [x] `Snapshot` round-trips losslessly through JSON (test).
- [x] `--dump-snapshots` flag produces files readable by hand.
- [x] Schema version field present.
- [x] ADR `007-snapshot-schema.md` written documenting the format (005 was already taken).
- [x] `docs/roadmap/README.md` table updated to `done`.
