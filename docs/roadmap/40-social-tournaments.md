# 40 — Social mechanics + tournaments

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Round out the "living world" pitch with social/cultural mechanics and curated events that give users reasons to come back and iterate.

## Scope
- New intrinsics for agents:
  - `BROADCAST(int)` — emits a small integer message in a radius.
  - `LISTEN()` — pops the most recent received message.
- Memory: agents gain a small per-instance memory bank (e.g. 16 ints) that persists across ticks.
- Curated events: scheduled simulations with constrained rule sets (e.g. "scarcity week", "predator vs prey").
- Tournament mode: a private sim with scoring + leaderboard.
- Shared culture metric: an aggregate stat tracking message diversity over time, exposed in `/stats`.

## Non-goals
- No emergent natural language analysis tooling — leave it to users.
- No real-time chat between users.

## Technical notes
- Broadcast uses an ephemeral per-tick radio grid; messages decay each tick.
- Tournament scoring is a configurable Python function checked into a `tournaments/` directory.

## Definition of Done
- [x] RadioGrid: broadcast(pos, message, radius) + listen(pos) + per-tick reset.
- [x] MemoryBank: 16-int per-agent persistent memory.
- [x] Agent struct gains MemoryBank field.
- [x] Tournament API: POST/GET /tournaments with leaderboard.
- [x] Tournament UI: /tournaments page with list + leaderboard display.
- [x] 4 tests: broadcast/listen, radio reset, memory bank default/read-write.
- [x] `docs/roadmap/README.md` table updated to `done`.
