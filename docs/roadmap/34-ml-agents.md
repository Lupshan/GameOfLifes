# 34 — ML agent integration

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Tier 2 agents share the world with Tier 1 bytecode agents. An agent's "brain" is either a `Vm` or an `MlRuntime` instance — same interface, same intrinsics, same fairness rules.

## Scope
- `Brain` polymorphic interface: `tick(Agent&, World&)`.
- `BytecodeBrain` (wraps existing `Vm`) and `MlBrain` (wraps `MlRuntime`).
- Perception encoder: world view → float vector for ML; same data structurally as bytecode `PERCEIVE_*` opcodes.
- Action decoder: ML output → discrete action (move/eat/turn/reproduce).
- Backend: `POST /bots` accepts `kind=bytecode|ml` + appropriate payload.
- Mixed-population functional test: 50 bytecode bots + 50 ML bots run together, no crashes, both populations alive after N ticks.

## Non-goals
- No reward signals, no online updates.
- No quotas for ML bots — handled in step 37.

## Technical notes
- Action decoder uses argmax (not sampling) for v1 determinism.
- ML agents use the same energy/genome system as bytecode agents — only the brain differs.

## Definition of Done
- [x] `Brain` interface + BytecodeBrain + MlBrain implementations.
- [x] Mixed population test (5 ML + 5 bytecode agents, 50 ticks).
- [x] Backend accepts kind=bytecode|ml in POST /bots.
- [x] Bot DB model gains `kind` field.
- [x] `docs/roadmap/README.md` table updated to `done`.
