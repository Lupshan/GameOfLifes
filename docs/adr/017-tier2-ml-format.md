# ADR 017 — Tier 2 ML: custom feed-forward format (not ONNX)

**Status**: accepted
**Date**: 2026-04-10

## Context

Tier 2 lets users submit frozen ML models. The roadmap originally specified ONNX via `onnxruntime`. However, `onnxruntime` is maintained by Microsoft (GAFAM policy) and is a heavy C++ dependency (~100MB).

## Decision

**Custom feed-forward network format** (`.golm` JSON files) with a pure C++ inference engine. No external ML library.

### Format
```json
{
  "layers": [
    {
      "input_size": 10,
      "output_size": 8,
      "activation": "relu",
      "weights": [/* input_size * output_size floats, row-major */],
      "biases": [/* output_size floats */]
    }
  ]
}
```

### Interface
- Input: 10 floats (terrain, food, water, agents_here, energy, hydration, speed, metabolism, swim, forage)
- Output: 6 floats (move, turn_left, turn_right, eat, drink, reproduce)
- Action selection: argmax of output vector

### Validation
- Shape check at load time
- Layer chain consistency (output_size[i] == input_size[i+1])

## Consequences

- Zero GAFAM dependencies. Pure C++ with nlohmann/json (already a dep).
- Limited to feed-forward networks (no RNNs, no attention). Sufficient for v1 — agents have simple perception.
- Users must export their models to the `.golm` format. A Python utility can be provided later.
- If more complex architectures are needed in v2, we can add them without changing the inference API.
- Inference is naive matmul — fast enough for small networks (<100 parameters per layer).
