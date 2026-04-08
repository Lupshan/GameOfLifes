# 31 — Stats / genealogy views

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Make the evolution visible. Add charts and a genealogy tree so users can see populations rise and fall, traits drift, and lineages branch.

## Scope
- `/stats` route with charts (using a small community-driven lib like `chartist` or hand-rolled SVG):
  - Population over time per bot.
  - Mean trait values over time.
  - Birth / death events per tick.
- `/genealogy/{bot_id}` route showing a lineage tree (limited depth) from `/lineage` API.
- Click on an agent in `/world` → side panel with its genome, traits, parent link.

## Non-goals
- No data export (CSV) — deferred.
- No filtering UI beyond bot selector.

## Technical notes
- Charts pull from a new `GET /stats?bot_id=&from=&to=` endpoint, served from precomputed aggregates (cron job updates them).
- Genealogy tree rendering: simple top-down layout, max 5 generations visible.

## Definition of Done
- [ ] Stats charts render with live data.
- [ ] Genealogy tree renders for a bot with multiple generations.
- [ ] Agent click → inspector works.
- [ ] `docs/roadmap/README.md` table updated to `done`.
