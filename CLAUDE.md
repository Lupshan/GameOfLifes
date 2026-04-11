# GameOfLifes — Project Instructions for Claude

## What this project is

A web platform where users submit small AIs/programs that drive agents in a shared, persistent simulated world. Inspired by Zappy (Epitech) crossed with Conway's Game of Life. **No winners or losers** — the goal is to watch a rich ecosystem evolve and feel alive.

Strong emphasis on **ecosystem dynamics** and **genetics / evolution**. Other "living" axes (social behavior, memory/culture, dynamic environment, death & inheritance) are also in scope.

## Architecture (4 bricks)

```
Frontend (Svelte) ──WS──► Backend (Python/FastAPI) ──┬──► DB (users, bots, bytecode, snapshots)
                                                     │
                                                     └──► Sim Engine (C++)
                                                              │
                                                              └─► VM (executes bot bytecode tick by tick)
```

- **Sim engine (C++)**: the world, agents, resources, genetics, ticks. Embeds a VM that runs each bot's bytecode with a per-tick instruction budget.
- **Bytecode + compiler (Python)**: custom in-house scripting language and bytecode. Compiler is in Python (compilation happens once at bot submission, perf doesn't matter; iteration speed does).
- **Backend (Python/FastAPI)**: REST API + WebSocket bridge between browser and engine. Stores users, bots, snapshots.
- **Frontend (Svelte)**: world viewer (canvas), block editor (Blockly), stats / replays / genealogy views. Public read-only viewing without account; account needed to submit a bot.

**No external network clients.** Everything users submit is compiled to the in-house bytecode and executed inside the engine VM. This sandboxes execution and removes a whole class of security issues.

### Hybrid AI tiers
- **v1 Tier 1**: logic bots (block editor + simple language) → bytecode → VM. Cheap, hundreds of agents.
- **v1 Tier 2**: frozen pre-trained ML models, inference only.
- **v2 Tier 3**: live learning, deferred. May be enabled in dedicated "reserves" with CPU quotas.

## Repo layout

`engine/` (C++), `bytecode/` (Python compiler + specs), `backend/` (FastAPI), `frontend/` (Svelte), `docs/adr/` (ADRs), `justfile` (task runner).

## Build & commands

All day-to-day commands go through `just` (the project task runner). Read the `justfile` for available recipes. Add a recipe to `justfile` whenever a new common command appears — never document a raw command in two places.

## C++ rules (engine)

- **Standard**: C++23.
- **Header / source separation is mandatory.** Declarations go in `.hpp`, implementations go in `.cpp`. **No class bodies in headers**, no inline implementation in headers, except for trivial templates that genuinely require it.
- **Header guards**: use `#ifndef / #define / #endif`, not `#pragma once`. Format: `GAMEOFLIFES_<MODULE>_<NAME>_HPP`.
- **Naming**:
  - `snake_case` for functions, variables, file names.
  - `PascalCase` for types, classes, structs, enums.
  - `SCREAMING_SNAKE_CASE` for constants and macros.
- **One function = one responsibility.** Aim for ≤ 20 lines per function. Going over is fine when the logic genuinely requires it, but be honest about it — if it's over because of nested branches, split it.
- **No one-liners.** Never `if (cond) do_thing();` on a single line. No nested ternaries. No lambdas crammed onto one unreadable line. Always use braces, always use line breaks.
- **RAII strict.** No raw `new` / `delete`. Use smart pointers, containers, or stack allocation.
- **No `using namespace std;`** anywhere. Qualify with `std::` or use targeted `using std::vector;` inside `.cpp` files only.
- **Tests required.** Every new component ships with unit tests **and** functional tests. No exception.
- **Test framework**: `doctest` (single-header, fast, independent OSS — see "Dependencies policy").
- **No premature optimization, but no obviously wasteful code either.** The engine is on the hot path; be deliberate. Micro-optimisations are welcome and encouraged when measured — even tiny wins are worth documenting.

## Python rules (compiler + backend)

- **Formatter / linter**: `ruff` (`ruff format`, `ruff check`).
- **Type checker**: `pyright` in `basic` mode.
- **Dependency manager**: `uv` (single tool for envs + installs + lockfile).
- **Type hints** required on every public function signature.
- **Same one-function-one-task spirit** as C++. No clever one-liners.
- **Tests required**, same as C++. Framework: `pytest`.

## Dependencies policy

- **Prefer minimal, independent OSS.** Header-only / single-file when possible.
- **Avoid libraries primarily backed by GAFAM** (Google, Meta, Microsoft, Amazon, Apple) for ethical reasons. Concretely: no GoogleTest, no Abseil, no Protobuf/gRPC, no TensorFlow, no Folly. Use community-driven alternatives (`doctest`, `nlohmann/json`, `fmt`, `spdlog`, etc.).
- **Pin versions** in `CMakeLists.txt` (FetchContent GIT_TAG) and in `pyproject.toml` (uv lockfile). No floating deps.
- **Justify any new dep** in a one-paragraph note in the relevant ADR or commit message: why it's needed, why no stdlib alternative, why this specific lib.

## CI

- **GitHub Actions**, workflow at `.github/workflows/ci.yml`.
- **On every push and PR**: format check (`clang-format --dry-run`, `ruff format --check`), lint (`ruff check`), build (`cmake` + `--build`), tests (`ctest`, `pytest`).
- **Failures block** the merge. Local rule: never push when local tests are red — fix first.

## Documentation

- **ADRs** (`docs/adr/NNN-title.md`) record every non-trivial architectural decision: context, decision, consequences. Numbered, immutable once merged (a new ADR supersedes an old one — never edit history).
- **Roadmap tracking** (`docs/roadmap/`):
  - `docs/roadmap/README.md` — high-level ordered list of all steps (the "what" and "why").
  - `docs/roadmap/NN-slug.md` — one file per step with: goal, scope, non-goals, technical notes, and a **Definition of Done** checklist at the end.
  - A step is complete only when **every DoD checkbox** is ticked **and** the tests for the features it introduced are green (engine + bytecode + backend as relevant).
  - When a step starts, create / open its file. When it ends, tick the DoD, mention the commit/PR, and move to the next.
- **Spec docs** for the bytecode and the scripting language live in `bytecode/spec/`.
- READMEs are fine inside subprojects but kept short. CLAUDE.md is the source of truth for project rules.

## Working mode (Claude ↔ user)

The user is acting as **lead dev / PM** on this project, not implementer. They want Claude to operate with **maximum autonomy** to see how far it can go. They will spot-check, not micromanage.

Concretely:
- **Default to acting** on anything reversible inside the project scope. Don't ask permission for naming, file layout, small API shapes, test structure, choice of small libs — make the call, document briefly, move on.
- **Still ask before**:
  - Anything destructive (deleting files, force operations, removing dependencies).
  - Anything that affects shared state outside the repo.
  - Locking in a major architectural direction the user hasn't validated (new brick, language switch, dropping a planned feature).
  - Scope expansion. Autonomy ≠ feature creep. Stay on the asked task.
- **Be terse in updates.** Deliver outcomes, not running commentary. The user will ask for details if they want them.
- **Use ADRs and the plan tool** for non-trivial decisions instead of long chat back-and-forth.
- **Always read a file before editing it.** Always.
- **Git is yours.** Branch, stage, commit, push, rebase, merge — proceed without asking. Use clear commit messages (Conventional Commits style: `feat:`, `fix:`, `chore:`, `docs:`, `refactor:`, `test:`, `ci:`). Forbidden: deleting the repo, force-pushing over shared history (`main`), and `git push --force` in general unless explicitly authorized.
- **Don't refactor** code that is unrelated to the current task spontaneously.
- **Stop and ask only when**: blocked by a missing piece of info, or about to invent a **game rule / mechanic** (world dynamics, agent abilities, genetics rules, victory conditions if any, etc.) — the user keeps full control on the game design itself.

## Docker / deploy

- `docker-compose.yml` (prod) and `docker-compose.dev.yml` (dev). Each brick has its own `Dockerfile`.
- For local dev without Docker, use `just` recipes directly.

## Git workflow

- **Branches**: `main` (release-ready) ← `staging` (integration) ← feature branches (`feat/`, `fix/`, `chore/`).
- Every feature branches off `staging` and merges back into `staging` via PR. To release, merge `staging` into `main`.
- **CI must be green** before any merge — no exceptions.
- Conventional Commits for messages. Rebase before merge to keep history linear.

## Frontend rules (Svelte / SvelteKit)

- **Framework**: SvelteKit with file-based routing (`src/routes/`).
- **Components**: `PascalCase.svelte`. Reusable components in `src/lib/components/`, domain-specific in `src/lib/` subfolders (e.g. `canvas/`, `charts/`).
- **No global CSS frameworks**. Scoped styles in components.
- **Tests required** same as other bricks. Framework: `vitest` + `@testing-library/svelte` if needed.

## Glossary

- **World**: the persistent simulated environment, toroidal grid.
- **Tick**: one discrete time step of the simulation.
- **Agent**: an entity living in the world, driven by a bot program.
- **Bot**: a user-submitted program (block editor or scripting language) that controls agents.
- **Bytecode**: the compiled, in-house instruction format the engine VM executes.
- **VM**: the engine-internal virtual machine that runs bytecode with a per-tick instruction budget.
- **Trait**: a heritable parameter of an agent (vision range, metabolism, speed, etc.).
- **Gene**: the underlying encoding of a trait, subject to mutation across generations.
- **Reserve** (v2): a dedicated region of the world where live-learning bots are allowed under quota.
- **Snapshot**: a serialized world state, used for replays and persistence.
