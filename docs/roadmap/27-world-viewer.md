# 27 — World viewer (canvas)

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
The flagship feature for the public: a canvas rendering of the live world, fed by the WebSocket stream from step 24. Anyone visiting the site sees the world tick.

## Scope
- `/world` route with a `<canvas>` element sized to viewport.
- WS client connects to `/ws/world`, parses snapshots, draws each tick.
- Render: terrain layer as background, food/water/mineral as colored cells, agents as colored dots (color from genome).
- HUD overlay: tick number, agent count, resource counts.
- Camera: pan + zoom with mouse.
- Performance target: 60fps with 500 agents on a 200x200 grid.

## Non-goals
- No agent inspection on click yet (step 31).
- No replay scrubbing (step 32).
- No 3D / WebGL — 2D canvas is enough for v1.

## Technical notes
- Render loop uses `requestAnimationFrame`, decoupled from WS tick rate (interpolation between snapshots optional).
- Color from genome: hash 3 bytes → RGB.
- Avoid React-style reactivity per cell — direct canvas draw is faster.

## Definition of Done
- [x] Live world renders via requestAnimationFrame (Canvas2D, decoupled from WS).
- [x] Pan (mouse drag) + zoom (wheel) works.
- [x] HUD shows tick, agent count, grid size, seed.
- [x] Public route `/world` accessible without auth.
- [x] Decode utilities tested (base64, bitfield unpacking).
- [x] `docs/roadmap/README.md` table updated to `done`.
