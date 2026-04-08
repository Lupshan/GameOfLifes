# 39 — On-demand private simulations

**Status**: not started (v2)
**Started**: —
**Finished**: —

## Goal
Let users spin up a private simulation with a chosen set of bots and config, separate from the persistent shared world. Useful for experiments, A/B testing of bot variants, and curated events.

## Scope
- `POST /sims` with body `{ name, config, bot_ids }` returns a simulation handle.
- Each private sim runs as a fresh engine subprocess.
- `GET /sims/{id}` and `WS /sims/{id}/world` mirror the public endpoints.
- Quotas: max concurrent sims per user, max ticks per sim, max wall-clock duration.
- UI: `/sandbox` route to launch and watch private sims.
- Sims auto-tear-down on completion or timeout.

## Non-goals
- No persistence of private sims past their lifetime (snapshots can be exported manually).
- No multi-user shared private sims.

## Technical notes
- Engine pool managed by `EngineService`, capped by available CPU.
- Per-sim isolation = one subprocess; no shared state.

## Definition of Done
- [ ] Private sim API + lifecycle management.
- [ ] UI to launch and watch.
- [ ] Quota enforcement.
- [ ] `docs/roadmap/README.md` table updated to `done`.
