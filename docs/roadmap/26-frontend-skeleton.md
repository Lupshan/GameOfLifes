# 26 — Frontend skeleton

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Stand up the SvelteKit project with build, test, lint, and a single placeholder page. Mirrors steps 03 and 19 — boring but necessary.

## Scope
- `frontend/` with SvelteKit + Vite + TypeScript.
- ESLint + Prettier configured.
- `vitest` for unit tests, `playwright` for e2e (skeleton only).
- One placeholder route `/` with project name + link to GitHub.
- `just build-frontend`, `just test-frontend`, `just dev-frontend`.
- CI frontend job added to workflow.

## Non-goals
- No actual UI yet.
- No real auth flow.

## Technical notes
- Pinned Node version via `.nvmrc` or `mise`.
- TypeScript strict mode.
- No CSS framework yet (decide in step 27 once we know what we need).

## Definition of Done
- [ ] `just dev-frontend` serves the placeholder page.
- [ ] `just test-frontend` passes.
- [ ] CI frontend job green.
- [ ] `docs/roadmap/README.md` table updated to `done`.
