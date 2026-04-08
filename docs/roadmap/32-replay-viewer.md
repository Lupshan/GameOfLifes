# 32 — Replay viewer

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Let users scrub through past world states. Same canvas as `/world`, but fed by paginated REST snapshots instead of WS.

## Scope
- `/replay` route with a timeline scrubber (start tick → current tick).
- Loads snapshots in chunks from `/snapshots`.
- Play / pause / speed controls (1x / 2x / 8x).
- Bookmarks: user can save tick numbers of interest (auth-only).

## Non-goals
- No editing past states.
- No diff view between two ticks.

## Technical notes
- Canvas rendering reused from step 27 (extract a render component).
- Prefetch ~10 snapshots ahead during playback to keep it smooth.

## Definition of Done
- [ ] Scrubbing works smoothly.
- [ ] Play/pause/speed controls work.
- [ ] Bookmarks persist.
- [ ] `docs/roadmap/README.md` table updated to `done`.
