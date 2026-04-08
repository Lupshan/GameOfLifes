# ADR 003 — Hybrid AI tiers

**Status**: accepted
**Date**: 2026-04-08

## Context

Users may want to submit anything from a Scratch-like block program to a trained neural network. Live online learning (a model that trains while running inside the world) is conceptually appealing but expensive in CPU, hard to sandbox, and breaks determinism.

## Decision

Adopt a **three-tier model**, with v1 limited to tiers 1 and 2:

- **Tier 1 — logic bots**: block editor or scripting language → bytecode → VM. Cheap, deterministic, hundreds of agents simultaneously possible. **In v1.**
- **Tier 2 — frozen ML models**: user uploads a pre-trained model, the engine runs inference only. More expensive than Tier 1; agent count capped. **In v1.**
- **Tier 3 — live learning**: bots train inside the world. **Deferred to v2.** Will likely live in dedicated "reserves" of the world with strict CPU quotas.

## Consequences

- v1 ships without solving the hardest problem (online training sandbox + compute budgeting).
- Population-scale **genetic evolution** (trait inheritance + mutation) already provides a strong "learning over time" feel without needing per-agent online learning.
- The bytecode design only has to support Tier 1 in v1; Tier 2 is essentially an inference primitive callable from bytecode. Tier 3 will require revisiting the VM's execution model.
- Users with ML backgrounds get a clear v2 promise instead of a flat "no".
