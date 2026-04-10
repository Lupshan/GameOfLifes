# ADR 016 — Full snapshots over WebSocket (no deltas for v1)

**Status**: accepted
**Date**: 2026-04-10

## Context

The WebSocket stream delivers world state to browser clients. Options: full snapshot per tick, or delta encoding (only changed cells/agents).

## Decision

**Full snapshots in v1.** Each WS message is a complete JSON snapshot.

A 200x200 grid with 500 agents produces ~50-100KB per snapshot. At 10 Hz, that's 0.5-1 MB/s per client. Acceptable for under 50 concurrent clients on a modern connection.

## Consequences

- Simple implementation: no diff algorithm, no client-side state reconstruction.
- Clients can join at any time without needing a "catch-up" protocol.
- Bandwidth scales linearly with clients. If this becomes a problem, add delta encoding in v2.
- Slow clients get dropped snapshots (queue backpressure, maxsize=100).
