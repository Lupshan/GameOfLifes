# 02 — CI pipeline

**Status**: done
**Started**: 2026-04-09
**Finished**: 2026-04-09

## Goal
Wire GitHub Actions so every push and PR runs format checks, lints, builds, and tests across all bricks. After this step, a red CI blocks merges and the contract "never push when local tests are red" is enforceable.

## Scope
- `.github/workflows/ci.yml` with jobs:
  - `fmt`: `clang-format --dry-run --Werror` on all `engine/**/*.{hpp,cpp}`, `ruff format --check` on Python bricks.
  - `lint`: `ruff check`, `pyright` on Python bricks.
  - `engine`: install clang/cmake, configure + build engine, run `ctest`.
  - `bytecode`: `uv sync` then `uv run pytest`.
  - `backend`: `uv sync` then `uv run pytest`.
- All jobs run in parallel; matrix on `ubuntu-latest` only for now.
- Cache strategy: `~/.cache/uv`, CMake build dir.
- Status badge in root `README.md`.

## Non-goals
- No frontend job yet (step 26 adds it).
- No deployment, no release automation (step 36).
- No Windows/macOS matrix.

## Technical notes
- Use official `actions/checkout@v4`, `astral-sh/setup-uv@v3`.
- Pin tool versions in workflow to match local dev (clang 18+, cmake 3.28+, python 3.12).
- Each brick job is independent — failure in one doesn't cancel the others (`fail-fast: false`).

## Definition of Done
- [x] `.github/workflows/ci.yml` exists and runs on `push` + `pull_request`.
- [x] All jobs green on a no-op PR.
- [x] A deliberately broken format / lint / test fails the right job (verified once).
- [x] CI badge in root README.
- [x] `docs/roadmap/README.md` table updated to `done`.
