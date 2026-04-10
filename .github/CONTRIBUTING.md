# Contributing to GameOfLifes

## Getting started

1. Fork and clone the repo
2. Install prerequisites: C++23 compiler, CMake 3.22+, Python 3.12+ with [uv](https://github.com/astral-sh/uv), Node.js 20+, [just](https://github.com/casey/just)
3. Run `just test` to verify everything builds and passes

## Development workflow

```sh
just fmt        # format all code before committing
just lint       # check for lint errors
just test       # run all tests
```

All code must pass format, lint, and test checks. CI enforces this on every push.

## Commit messages

We use [Conventional Commits](https://www.conventionalcommits.org/):

```
feat(engine): add broadcast mechanic
fix(backend): prevent N+1 query on bot list
docs(adr): add ADR-019 for reserve system
test(bytecode): cover edge cases in codegen
chore(ci): update Node version in workflow
refactor(frontend): extract canvas renderer
```

Scope is one of: `engine`, `backend`, `frontend`, `bytecode`, `ci`, `docs`.

## Code style

- **C++**: `clang-format` (config at `.clang-format`). C++23, header/source separation, `snake_case` functions, `PascalCase` types.
- **Python**: `ruff format` + `ruff check`. Type hints on all public functions. `pyright` in basic mode.
- **TypeScript/Svelte**: `prettier`. Strict TypeScript.

See [CLAUDE.md](../CLAUDE.md) for full conventions.

## Pull requests

- One logical change per PR
- Tests required for new features and bug fixes
- Fill in the PR template
- CI must be green before merge

## Architecture decisions

Non-trivial decisions are recorded as ADRs in `docs/adr/`. If your change introduces a new dependency, changes the data model, or affects the architecture, write an ADR.

## Dependencies policy

- Prefer minimal, independent OSS libraries
- No libraries primarily backed by GAFAM (Google, Meta, Microsoft, Amazon, Apple)
- Pin versions. Justify any new dependency in the commit message or an ADR.

## Game design

Bot abilities, world mechanics, genetics rules, and scoring are designed by the maintainer. If you want to propose a new game mechanic, open a feature request issue first.
