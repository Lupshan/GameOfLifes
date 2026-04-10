# Roadmap — index

Living document. Each step produces something visible and testable on its own. Order is meaningful: every step builds on the previous one. Move a step only if there's a good reason and write it down (in this file or in the step's own file).

Every step has its own detail file in this folder (`NN-slug.md`) ending with a **Definition of Done** checklist. A step is complete only when every DoD box is ticked **and** the tests for the features it introduced are green.

## Phase 0 — Bootstrap

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 01 | Repo bootstrap                    | [01-repo-bootstrap.md](01-repo-bootstrap.md)                  | done        |
| 02 | CI pipeline                       | [02-ci-pipeline.md](02-ci-pipeline.md)                        | done        |

## Phase 1 — Engine core (C++)

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 03 | Engine skeleton (CMake + doctest) | [03-engine-skeleton.md](03-engine-skeleton.md)                | done        |
| 04 | World grid + primitives           | [04-world-grid.md](04-world-grid.md)                          | done        |
| 05 | RNG + config                      | [05-rng-config.md](05-rng-config.md)                          | done        |
| 06 | Agents + tick loop                | [06-agents-tick-loop.md](06-agents-tick-loop.md)              | done        |
| 07 | World snapshot serialization      | [07-snapshot-serialization.md](07-snapshot-serialization.md)  | done        |

## Phase 2 — Genetics & ecosystem

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 08 | Genome, traits, mutation          | [08-genome-traits.md](08-genome-traits.md)                    | done        |
| 09 | Reproduction & inheritance        | [09-reproduction.md](09-reproduction.md)                      | done        |
| 10 | Multi-resource ecosystem          | [10-ecosystem-depth.md](10-ecosystem-depth.md)                | done        |
| 11 | Environmental cycle               | [11-environmental-cycle.md](11-environmental-cycle.md)        | done        |

## Phase 3 — Bytecode & VM

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 12 | Bytecode spec                     | [12-bytecode-spec.md](12-bytecode-spec.md)                    | done        |
| 13 | VM execution engine (C++)         | [13-vm-engine.md](13-vm-engine.md)                            | done        |
| 14 | Agent ↔ VM binding                | [14-agent-vm-binding.md](14-agent-vm-binding.md)              | done        |

## Phase 4 — Language & compiler (Python)

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 15 | Language grammar spec             | [15-language-spec.md](15-language-spec.md)                    | done        |
| 16 | Lexer + parser                    | [16-lexer-parser.md](16-lexer-parser.md)                      | done        |
| 17 | AST → bytecode codegen            | [17-codegen.md](17-codegen.md)                                | done        |
| 18 | Compiler CLI + round-trip tests   | [18-compiler-cli.md](18-compiler-cli.md)                      | done        |

## Phase 5 — Backend (FastAPI)

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 19 | Backend skeleton                  | [19-backend-skeleton.md](19-backend-skeleton.md)              | done        |
| 20 | DB schema + migrations            | [20-db-schema.md](20-db-schema.md)                            | done        |
| 21 | User auth                         | [21-user-auth.md](21-user-auth.md)                            | not started |
| 22 | Bot submission API                | [22-bot-submission-api.md](22-bot-submission-api.md)          | not started |
| 23 | Engine bridge                     | [23-engine-bridge.md](23-engine-bridge.md)                    | not started |
| 24 | WebSocket world stream            | [24-websocket-stream.md](24-websocket-stream.md)              | not started |
| 25 | Snapshot/replay storage API       | [25-replay-api.md](25-replay-api.md)                          | not started |

## Phase 6 — Frontend (Svelte)

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 26 | Frontend skeleton                 | [26-frontend-skeleton.md](26-frontend-skeleton.md)            | not started |
| 27 | World viewer (canvas)             | [27-world-viewer.md](27-world-viewer.md)                      | not started |
| 28 | Auth + bot management UI          | [28-auth-ui.md](28-auth-ui.md)                                | not started |
| 29 | Blockly block editor              | [29-block-editor.md](29-block-editor.md)                      | not started |
| 30 | End-to-end bot submission flow    | [30-submission-flow.md](30-submission-flow.md)                | not started |
| 31 | Stats / genealogy views           | [31-stats-views.md](31-stats-views.md)                        | not started |
| 32 | Replay viewer                     | [32-replay-viewer.md](32-replay-viewer.md)                    | not started |

## Phase 7 — Tier 2 (frozen ML)

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 33 | ML model loader (ONNX)            | [33-ml-loader.md](33-ml-loader.md)                            | not started |
| 34 | ML agent integration              | [34-ml-agents.md](34-ml-agents.md)                            | not started |

## Phase 8 — Production

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 35 | Observability                     | [35-observability.md](35-observability.md)                    | not started |
| 36 | Deployment                        | [36-deployment.md](36-deployment.md)                          | not started |
| 37 | Launch hardening                  | [37-launch-hardening.md](37-launch-hardening.md)              | not started |

## Phase 9 — v2

| #  | Step                              | File                                                          | Status      |
|----|-----------------------------------|---------------------------------------------------------------|-------------|
| 38 | Tier 3 — live learning + reserves | [38-tier3-live-learning.md](38-tier3-live-learning.md)        | not started |
| 39 | On-demand private simulations     | [39-private-sims.md](39-private-sims.md)                      | not started |
| 40 | Social mechanics + tournaments    | [40-social-tournaments.md](40-social-tournaments.md)          | not started |

## Detail file template

Each `NN-slug.md` follows this skeleton:

```markdown
# NN — Step name

**Status**: not started | in progress | done
**Started**: YYYY-MM-DD
**Finished**: YYYY-MM-DD

## Goal
One paragraph: what this step delivers and why it exists in the order it's in.

## Scope
- bullet list of what's included

## Non-goals
- bullet list of what's deliberately *not* in this step (to prevent scope creep)

## Technical notes
Design choices, file layout, key types, anything worth recording. Link to ADRs if a decision was made.

## Definition of Done
- [ ] Concrete checkbox 1
- [ ] Concrete checkbox 2
- [ ] All tests for features introduced in this step pass (`just test`)
- [ ] CI is green on the branch
- [ ] `docs/roadmap/README.md` table updated to `done`
```
