# ADR 004 — No external network clients

**Status**: accepted
**Date**: 2026-04-08

## Context

Zappy uses a public TCP protocol that lets any external program connect as a client. Reusing that model would let advanced users plug their own AI in any language. But it forces us to expose a network surface, deal with authentication, rate limiting, malicious clients, and a public protocol contract we can't easily evolve.

## Decision

**No external network clients.** Every bot a user submits — whether built in the block editor or written in the in-house scripting language — is compiled to the in-house bytecode and executed by the engine VM.

The engine ↔ backend channel exists, but it is **private** (in-process or local IPC), not a public protocol.

## Consequences

- No public network attack surface on the engine.
- The bytecode is an internal contract; we can change it freely without breaking external users.
- Advanced users lose the ability to run their AI in their own process / language. They must express their bot in the in-house language. Acceptable trade-off in v1; the language can be made expressive enough for serious bots.
- Architecture is simpler: one execution model, one entry point, one set of tests.
