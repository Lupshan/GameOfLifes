# 33 — ML model loader (ONNX)

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Open Tier 2: let users submit a frozen ML model (ONNX format) instead of bytecode. The engine loads it and runs inference per agent per tick. This step is just the loader and inference path — integration with agents is step 34.

## Scope
- Dep: `onnxruntime` C++ API (community-friendly enough; document in ADR if concerns).
- `engine/include/engine/ml_runtime.hpp` — loads `.onnx` files, validates input/output shapes, runs inference.
- Required model interface: input = fixed-length float vector (perception), output = fixed-length float vector (action probabilities).
- Validation rejects models with wrong shapes.
- Tests: load a tiny test model, run inference, check determinism.

## Non-goals
- No training inside the engine (Tier 3, step 38).
- No GPU inference for v1.
- No model versioning beyond a hash.

## Technical notes
- ADR `012-tier2-ml.md` documenting ONNX choice and model interface.
- Inference budget: a model that exceeds N ms per inference is rejected at load time.

## Definition of Done
- [ ] ONNX runtime integrated.
- [ ] Loader + validation + inference tested.
- [ ] ADR 012 written.
- [ ] `docs/roadmap/README.md` table updated to `done`.
