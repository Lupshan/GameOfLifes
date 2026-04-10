# 23 — Engine bridge

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

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
- [x] Engine `--ipc` mode implemented (stdin/stdout JSON, tick loop with command polling).
- [x] Backend EngineProcess (subprocess spawn, stdin/stdout, event queue) + EngineService.
- [x] Protocol tests (command/event serialization).
- [x] Restart-on-crash logic in EngineService dispatcher.
- [x] ADR 014 written (stdin/stdout JSON IPC).
- [x] `docs/roadmap/README.md` table updated to `done`.
