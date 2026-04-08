# 24 — WebSocket world stream

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Expose the live world to browsers. The backend forwards engine snapshots to all subscribed WebSocket clients in real time.

## Scope
- `WS /ws/world` — clients receive a snapshot per tick (or throttled).
- Initial message on connect: full world state. Subsequent messages: deltas (or full snapshots if delta is too complex for v1).
- Multiple concurrent clients supported via a fan-out pubsub (in-memory `asyncio.Queue` per client).
- Auth optional (read-only viewing without account, per CLAUDE.md).
- Tests: connect, receive initial state, receive an update.

## Non-goals
- No room/region subscription (whole-world only for v1).
- No replay over WS (REST in step 25).
- No client-to-server commands beyond ping/pong.

## Technical notes
- Decision to record: full snapshots vs deltas. Default to full for v1; revisit if bandwidth hurts.
- Subscriber list lives in `EngineService`; backend disconnects on client error.

## Definition of Done
- [ ] WS endpoint implemented.
- [ ] Multi-client fan-out tested.
- [ ] Disconnect cleanup verified (no leaks).
- [ ] `docs/roadmap/README.md` table updated to `done`.
