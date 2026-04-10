# 25 — Snapshot/replay storage API

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Persist periodic snapshots and expose them via REST so users can scrub through past world states.

## Scope
- Backend periodically (e.g. every 100 ticks) stores a full snapshot to disk + DB row.
- `GET /snapshots?from=&to=` — list snapshot metadata.
- `GET /snapshots/{id}` — fetch a snapshot blob.
- `GET /lineage?bot_id=&from_tick=&to_tick=` — fetch lineage events.
- Disk storage layout: `data/snapshots/YYYY/MM/DD/tick_NNNN.json.zst` (zstd-compressed).
- Pruning policy: keep last N days (configurable).

## Non-goals
- No long-term archival to S3.
- No full event sourcing — periodic snapshots only.

## Technical notes
- zstd via `python-zstandard` (community-driven).
- Snapshot compression saves ~10x on JSON.

## Definition of Done
- [x] Snapshot storage service (write_snapshot, read_snapshot, zstd compression).
- [x] All endpoints: GET /snapshots, GET /snapshots/{id}, GET /lineage.
- [x] Pruning works (prune_snapshots with configurable keep_days).
- [x] 6 tests: write/read, compression ratio, prune, REST endpoints.
- [x] `docs/roadmap/README.md` table updated to `done`.
