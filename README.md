# GameOfLifes

[![CI](https://github.com/Lupshan/GameOfLifes/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/Lupshan/GameOfLifes/actions/workflows/ci.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://en.cppreference.com/)
[![Python 3.12+](https://img.shields.io/badge/Python-3.12+-3776ab)](https://www.python.org/)
[![SvelteKit](https://img.shields.io/badge/SvelteKit-2-ff3e00)](https://kit.svelte.dev/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A web platform where users submit small AIs that drive agents in a shared, persistent simulated world. Inspired by [Zappy](https://github.com/Epitech) crossed with Conway's Game of Life — **no winners, no losers**, just a rich ecosystem to watch evolve, with a strong focus on **genetics and emergent behavior**.

## What happens

Users write bot programs using a visual block editor (Blockly) or a small scripting language. Bots are compiled to an in-house bytecode and executed inside a sandboxed VM in the engine. Agents eat, drink, move, reproduce, mutate, and die. Over time, traits drift, niches form, and the world comes alive.

## Architecture

```
Frontend (Svelte) ──WS──► Backend (Python/FastAPI) ──┬──► PostgreSQL
                                                     └──► Sim Engine (C++)
                                                              └─► VM (bytecode)
```

| Brick | Tech | Role |
|-------|------|------|
| **Engine** | C++23 | World simulation, agents, genetics, ticks, bytecode VM |
| **Bytecode** | Python | Custom scripting language + compiler (source → bytecode) |
| **Backend** | FastAPI | REST API, WebSocket bridge, auth, bot storage, snapshots |
| **Frontend** | SvelteKit | Canvas world viewer, Blockly editor, stats, replays |

## Quick start

### Prerequisites

- [just](https://github.com/casey/just) (task runner)
- C++23 compiler + CMake 3.22+
- Python 3.12+ with [uv](https://github.com/astral-sh/uv)
- Node.js 20+

### Development

```sh
just build      # build the C++ engine
just test       # run all tests (engine + bytecode + backend + frontend)
just fmt        # format all code (clang-format + ruff + prettier)
just lint       # lint all code (ruff + pyright)
just run        # start the simulation
```

### Docker (production)

```sh
cp .env.example .env
# Fill in POSTGRES_PASSWORD and JWT_SECRET (see .env.example)
docker compose up --build
```

- Frontend: http://localhost
- Backend API: http://localhost:8000
- Prometheus: http://localhost:9090

## Project structure

```
GameOfLifes/
├── engine/          # C++ sim engine + VM
├── bytecode/        # Scripting language spec + compiler
├── backend/         # FastAPI REST + WebSocket server
├── frontend/        # SvelteKit web app
├── docs/
│   ├── adr/         # Architecture Decision Records (18)
│   ├── roadmap/     # 40-step build plan (all completed)
│   └── audit/       # Security & code audit findings
├── .github/workflows/
│   ├── ci.yml       # Format, lint, build, test on every push
│   ├── release.yml  # Docker build + GitHub Release on v* tags
│   ├── claude-code-review.yml
│   └── security-review.yml
├── justfile         # Unified task runner
├── docker-compose.yml
└── CLAUDE.md        # Detailed project rules and conventions
```

## AI tiers

| Tier | Description | Status |
|------|-------------|--------|
| **Tier 1** | Logic bots (block editor + scripting language → bytecode → VM) | v1 |
| **Tier 2** | Frozen pre-trained ML models (ONNX), inference only | v1 |
| **Tier 3** | Live learning in dedicated "reserves" with CPU quotas | v2 |

## Contributing

See [CONTRIBUTING.md](.github/CONTRIBUTING.md) for guidelines on how to contribute.

## License

[MIT](LICENSE)
