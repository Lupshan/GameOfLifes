# 40 — Social mechanics + tournaments

**Status**: not started (v2)
**Started**: —
**Finished**: —

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
- [ ] Broadcast/listen + memory implemented + tested.
- [ ] At least one curated event runs end-to-end.
- [ ] Tournament mode + leaderboard.
- [ ] Culture diversity metric exposed.
- [ ] `docs/roadmap/README.md` table updated to `done`.
