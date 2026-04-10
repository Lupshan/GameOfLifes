# 30 — End-to-end bot submission flow

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Connect every brick: a user edits blocks, hits "submit", the backend compiles + stores, the engine loads the bot, agents driven by the user's bot appear in the live world. The first time the project is *actually* what it claims to be.

## Scope
- "Submit" button on `/edit` calls `POST /bots` with the generated source.
- Compile errors shown inline next to the editor.
- On success, redirect to `/me/bots/{id}` showing the bot's status.
- "Publish" button calls `POST /bots/{id}/publish`, which triggers `EngineService.load_bot`.
- The newly loaded bot spawns N agents in the world; they're visible in `/world`.
- E2e test (Playwright): full flow from signup to seeing your bot's agents on screen.

## Non-goals
- No bot deletion yet (low priority, deferred).
- No A/B testing of bot variants.

## Technical notes
- E2e test runs against a docker-composed stack (backend + engine + frontend) — see step 36 for the compose file. Until then, run components manually.
- Spawn count for a published bot: configured in `WorldConfig`.

## Definition of Done
- [x] Full flow wired: edit → submit → compile → bot detail → publish → world.
- [x] Compile errors shown inline in editor.
- [x] Bot detail page with source, compile status, publish button.
- [ ] Playwright e2e test deferred to deployment step (requires full stack).
- [x] `docs/roadmap/README.md` table updated to `done`.
