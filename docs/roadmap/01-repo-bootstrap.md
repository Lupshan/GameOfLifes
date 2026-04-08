# 01 — Repo bootstrap

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Set up the shared plumbing every brick will rely on: task runner, formatters, linters, language toolchains, top-level layout. After this step, anybody can clone the repo and run `just fmt`, `just lint` on an empty codebase without errors.

## Scope
- `justfile` at repo root with placeholder recipes: `build`, `test`, `run`, `fmt`, `lint`, plus per-brick variants (`build-engine`, `test-engine`, `test-bytecode`, `test-backend`).
- `.clang-format` (already present, verify settings match CLAUDE.md C++ rules).
- `.gitignore` covering `build/`, `__pycache__/`, `.venv/`, `node_modules/`, `dist/`, `.uv/`, snapshot dumps.
- Top-level directories created with empty `.gitkeep`: `engine/`, `bytecode/`, `backend/`, `frontend/`, `docs/adr/`, `docs/roadmap/`.
- `pyproject.toml` at repo root **or** under `bytecode/` and `backend/` (decide: per-brick). Configure `ruff` (format + check) and `pyright` (basic mode).
- `uv` lockfile committed for each Python brick.
- README.md at repo root with one-paragraph project pitch + pointer to CLAUDE.md and `docs/roadmap/`.

## Non-goals
- No actual source code, no CMake yet (step 03), no FastAPI app (step 19), no Svelte (step 26).
- No pre-commit hooks (optional later).

## Technical notes
- `just` recipes call into nested tools (`cmake --build build/`, `uv run pytest`, etc.). Recipes act as the single source of truth for commands.
- Decision recorded: per-brick `pyproject.toml` (one in `bytecode/`, one in `backend/`) rather than a single root one — keeps deps isolated.
- Ruff config: line length 100, target Python 3.12.
- Pyright: `basic` mode, `strict` for `bytecode/compiler/` and `backend/app/`. Exposed via `just typecheck` (not part of `just lint` to keep lint fast).
- Default branch is `main`. The `master` name has been removed everywhere.

## Definition of Done
- [x] `justfile` exists with all listed recipes (no-op if needed).
- [x] `just fmt` and `just lint` exit 0 on a freshly cloned repo.
- [x] `pyproject.toml` files exist in `bytecode/` and `backend/` with ruff + pyright config.
- [x] `uv sync` works in both Python bricks.
- [x] Root `README.md` exists.
- [x] `.gitignore` covers all listed patterns.
- [x] `docs/roadmap/README.md` table updated to `done`.
