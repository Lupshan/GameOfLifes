# ADR 001 — Four-brick architecture

**Status**: accepted
**Date**: 2026-04-08

## Context

GameOfLifes needs to host a persistent shared simulated world, accept user-submitted bots, and let people watch the world evolve through a web interface. We need a layout that lets us work in our strong languages (C++, Python), keeps the simulation core decoupled from the web stack, and makes the system easy to extend brick by brick.

## Decision

Split the system into four bricks:

1. **Sim Engine (C++)** — owns the world state, the tick loop, and an embedded VM that executes bot bytecode.
2. **Bytecode + compiler (Python)** — defines the in-house scripting language, the bytecode opcodes, and the source-to-bytecode compiler. Compilation happens at bot submission time, not in the hot path.
3. **Backend (Python / FastAPI)** — REST API and WebSocket bridge between the browser and the engine. Owns users, bot storage, and snapshots.
4. **Frontend (Svelte)** — public viewer (canvas), block editor (Blockly), stats / replays / genealogy views.

The engine and backend communicate through a private IPC channel (not a public TCP protocol — see ADR 004).

## Consequences

- Each brick can be developed and tested in isolation.
- C++ stays on the hot path; Python handles the parts where iteration speed matters more than raw perf.
- The frontend learning curve is contained to one brick — the user is new to web dev and we don't want web concerns leaking into the engine.
- Adds the cost of an IPC layer between engine and backend, but the cost is paid once and the isolation is worth it.
