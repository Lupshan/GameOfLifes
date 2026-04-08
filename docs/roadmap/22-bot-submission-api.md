# 22 — Bot submission API

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Let authenticated users submit bot source code. The backend invokes the Python compiler, stores both source and bytecode, and exposes a list/get API.

## Scope
- `POST /bots` (auth) — body `{ name, source }`. Compiles synchronously, returns `{ id, status, errors? }`.
- `GET /bots` (auth) — list user's bots.
- `GET /bots/{id}` (auth or public if bot is published) — metadata + source.
- `POST /bots/{id}/publish` — marks bot as live in the world.
- Compilation runs in-process by importing `bytecode.compiler` (since both bricks are Python).
- Tests: submit valid source, submit broken source (error returned), list, publish.

## Non-goals
- No version diffing UI.
- No quotas yet (step 37).
- No engine deployment of the bot — that's step 23.

## Technical notes
- Bot bytecode stored as a blob in DB; could move to object storage later if size grows.
- Compilation errors mapped to a structured response (`{ line, col, message }`).
- Backend depends on `bytecode` package — added to `backend/pyproject.toml` as a path dep.

## Definition of Done
- [ ] All endpoints implemented + tested.
- [ ] Compile errors returned cleanly.
- [ ] Cross-brick path dep works.
- [ ] `docs/roadmap/README.md` table updated to `done`.
