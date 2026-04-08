# 09 — Reproduction & inheritance

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Close the evolutionary loop: agents that survive and accumulate energy reproduce, offspring inherit a mutated genome, dead agents leave their lineage behind. After this step, a multi-generation run shows trait drift over time.

## Scope
- Reproduction rule: when `agent.energy >= traits.reproduction_threshold`, spawn a child on a random adjacent free cell, parent loses half its energy, child starts with the other half.
- Child genome = `mutate(parent.genome)`.
- Each agent has a `parent_id` and `generation` field.
- Lineage log: emit a JSON-line per birth/death event to a `lineage.jsonl` file (used later by genealogy view).
- Functional test: seeded run of N ticks shows generation count increases and trait distribution shifts under selection pressure (e.g. food-scarce world favors low metabolism).

## Non-goals
- No sexual reproduction (asexual only for now; document as a deliberate v1 simplification).
- No genealogy UI (step 31).
- No species clustering / speciation analytics.

## Technical notes
- Birth event: `{ "tick": T, "type": "birth", "id": ..., "parent": ..., "gen": ... }`.
- Death event: `{ "tick": T, "type": "death", "id": ..., "cause": "starvation" }`.
- ID generator monotonic on `World`.
- Reproduction step happens after movement, before reap.

## Definition of Done
- [ ] Reproduction rule implemented and tested.
- [ ] Lineage log written and parseable.
- [ ] Multi-generation functional test passes (selection pressure observable).
- [ ] `docs/roadmap/README.md` table updated to `done`.
