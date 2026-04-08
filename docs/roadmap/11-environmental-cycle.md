# 11 — Environmental cycle

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Add a time-varying environment so that the world isn't a static fitness landscape. Day/night and seasons make some traits situationally useful and force populations to adapt over time.

## Scope
- `WorldClock` tracking `tick_count`, day phase, season.
- Day/night: affects vision range and food regrowth.
- Seasons: affects ambient resource spawn rates.
- Configurable cycle lengths in `WorldConfig`.
- Functional test: identical seed runs with cycle on vs off produce divergent populations.

## Non-goals
- No weather events (storms, droughts) — interesting but deferred.
- No agent-perceived time (agents don't "know" the season; environment just changes).

## Technical notes
- Cycle math is pure (`day_phase(tick) -> double in [0,1]`), tested independently.
- All env-cycle effects funnel through helper functions consumed by tick loop, never inline conditionals.

## Definition of Done
- [ ] `WorldClock` + cycle helpers + tests.
- [ ] Tick loop reads cycle for resource spawn + vision.
- [ ] Functional divergence test passes.
- [ ] `docs/roadmap/README.md` table updated to `done`.
