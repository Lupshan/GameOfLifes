# 19 — Backend skeleton

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Stand up a FastAPI app with the project layout, dependency wiring, and a single `/health` endpoint. Like step 03 for the engine — boring but necessary.

## Scope
- `backend/pyproject.toml` with deps: `fastapi`, `uvicorn`, `pydantic`, `sqlmodel` (or `sqlalchemy`), `python-jose`, `passlib`, `pytest`, `httpx` (test client).
- `backend/app/main.py` — FastAPI app factory.
- `backend/app/api/health.py` — `GET /health` returning `{"status": "ok"}`.
- `backend/app/config.py` — settings loaded from env (pydantic-settings).
- `backend/tests/test_health.py` — uses `httpx.AsyncClient`.
- `just run-backend`, `just test-backend` recipes.

## Non-goals
- No DB yet (step 20).
- No auth (step 21).
- No engine bridge (step 23).

## Technical notes
- App factory pattern (`create_app()`) so tests can spin up isolated instances.
- Settings from env, with `.env.example` checked in.
- Strict pyright on `backend/app/`.

## Definition of Done
- [x] FastAPI app starts via `just run-backend`.
- [x] `GET /health` returns 200.
- [x] Test passes (httpx AsyncClient).
- [x] CI backend job green.
- [x] `docs/roadmap/README.md` table updated to `done`.
