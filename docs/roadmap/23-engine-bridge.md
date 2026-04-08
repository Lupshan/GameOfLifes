# 23 — Engine bridge

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Run the C++ engine as a long-lived subprocess managed by the backend, with a structured IPC channel for sending commands (load bot, pause, resume) and receiving snapshots.

## Scope
- `backend/app/engine/process.py` — manages engine subprocess lifecycle (spawn, monitor, restart on crash).
- IPC: line-delimited JSON over the engine's stdin/stdout. Commands like `{"cmd":"load_bot","id":42,"bytecode":"..."}` and responses like `{"event":"snapshot","tick":..., ...}`.
- Engine side: `gameoflifes_engine --ipc` mode reads commands from stdin and emits events to stdout.
- Backend service `EngineService` with methods `load_bot`, `unload_bot`, `pause`, `resume`, `subscribe_snapshots()`.
- Tests: spin up engine subprocess in a fixture, send a command, assert response.

## Non-goals
- No multi-engine pool / sharding.
- No engine hot-reload.

## Technical notes
- ADR `011-engine-ipc.md` documenting the choice of stdin/stdout JSON over alternatives (sockets, shared memory).
- Backpressure: snapshot emission can be throttled by an env var (`SNAPSHOT_HZ`).
- Engine subprocess is supervised — crash → log + restart with last snapshot.

## Definition of Done
- [ ] Engine `--ipc` mode implemented.
- [ ] Backend can spawn + control engine.
- [ ] Round-trip command/event tested.
- [ ] Restart-on-crash tested.
- [ ] ADR 011 written.
- [ ] `docs/roadmap/README.md` table updated to `done`.
