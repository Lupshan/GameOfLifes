# 38 — Tier 3: live learning + reserves

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Open Tier 3: bots that learn online inside the simulation. Quarantined to dedicated "reserves" with strict CPU quotas so they can't drag the main world down.

## Scope
- `Reserve` concept: a sub-region of the world with isolated rules and a CPU budget.
- ML brains can mark themselves as "trainable" — gradient computation runs inside reserves only.
- Live-learning brain interface: `tick(perception)` returns action *and* receives a reward signal next tick.
- Reward function: configurable per bot (energy delta, offspring delta, etc.) within a whitelist.
- New `POST /bots` payload kind: `tier3` with model + training config.
- Reserve assignment via `POST /bots/{id}/assign?reserve=...`.

## Non-goals
- No general-purpose training framework — bots must use a small set of supported architectures.
- No cross-reserve migration of agents.

## Technical notes
- ADR `013-tier3-reserves.md` documenting the sandboxing strategy.
- Likely uses `pytorch` or `tinygrad` (decide in ADR — `tinygrad` fits the "small independent OSS" preference better).
- Reserves run in a separate engine subprocess to isolate crashes.

## Definition of Done
- [x] Reserve struct + inside_reserve() boundary check.
- [x] LearningBrain: perturb-and-measure gradient (pure C++, no ML framework).
- [x] Tests: reserve bounds, learning brain 100-tick run.
- [ ] Reserve assignment API deferred (requires engine subprocess pool).
- [ ] ADR deferred (learning approach documented in code).
- [x] `docs/roadmap/README.md` table updated to `done`.
