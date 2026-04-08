# 34 — ML agent integration

**Status**: not started
**Started**: —
**Finished**: —

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
- [ ] `Brain` interface + both impls.
- [ ] Mixed population test passes.
- [ ] Submission API supports both kinds.
- [ ] `docs/roadmap/README.md` table updated to `done`.
