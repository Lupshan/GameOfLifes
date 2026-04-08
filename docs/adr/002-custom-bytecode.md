# ADR 002 — Custom in-house bytecode

**Status**: accepted
**Date**: 2026-04-08

## Context

Users submit programs that drive agents inside the world. We need a way to execute them safely (untrusted code), efficiently (potentially hundreds of agents per tick), and uniformly (block editor and text language must converge to the same execution model).

Options considered:
- Embed an existing language runtime (Lua, WASM via wasm3, restricted Python).
- Design our own language and bytecode.

## Decision

Design a **custom in-house scripting language and bytecode**, executed by a VM embedded in the C++ engine. The compiler (source → bytecode) lives in `bytecode/compiler/` and is written in Python.

The block editor compiles to the same bytecode, so there is exactly one execution path.

## Consequences

- **Sandbox by construction**: the VM only knows the opcodes we define. No filesystem, no network, no syscalls. A whole class of security issues disappears.
- **Per-tick instruction budget** is trivial to enforce in the VM, replacing Zappy's "time unit" concept and protecting the world from runaway bots.
- **Full design freedom** — we can shape the language around the simulation primitives (look, move, eat, sense gradient, broadcast) instead of bolting them onto a general-purpose runtime.
- **More work upfront** to build the language, the compiler, and the VM. Mitigated by keeping the language deliberately small in v1.
- **No learning curve from a third-party runtime** for users — but they have to learn our language. Block editor compensates for non-coders.
