# ADR 014 — Engine IPC: stdin/stdout line-delimited JSON

**Status**: accepted
**Date**: 2026-04-10

## Context

The backend needs to communicate with the C++ engine subprocess. Options considered: TCP sockets, Unix domain sockets, shared memory, stdin/stdout pipes.

## Decision

**Line-delimited JSON over stdin/stdout pipes.**

- Backend spawns `gameoflifes_engine --ipc` as a subprocess.
- Commands: backend writes JSON objects to engine's stdin (one per line).
- Events: engine writes JSON objects to its stdout (one per line).
- Engine runs a tick loop, emitting a snapshot event after each tick.
- Commands processed between ticks (non-blocking check of stdin buffer).

### Command format
```json
{"cmd": "load_bot", "bot_id": "abc123", "bytecode": "<base64>"}
{"cmd": "pause"}
{"cmd": "resume"}
{"cmd": "shutdown"}
```

### Event format
```json
{"event": "ready"}
{"event": "ack", "cmd": "pause"}
{"event": "snapshot", "tick": 42, "agents": [...], ...}
{"event": "error", "error": "message"}
```

## Consequences

- **Simplest possible IPC**: no socket setup, no port allocation, no firewall issues. Works identically on all platforms.
- **JSON is already the snapshot format** (ADR 007), so no serialization mismatch.
- **Backpressure**: engine emits ~10 snapshots/sec. At ~50-100KB per snapshot, that's 0.5-1 MB/s through the pipe. Well within OS pipe buffer capacity.
- **No multi-client**: only one backend process talks to one engine. Horizontal scaling requires multiple engine subprocesses (Phase 8 concern).
- **Crash recovery**: if the engine process exits unexpectedly, the backend detects it (EOF on stdout reader) and restarts a fresh engine.
