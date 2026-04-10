# ADR-018: Frontend Stack

**Date**: 2026-04-10
**Status**: Accepted

## Context

The frontend needs to:

1. Render a live simulation world (canvas-based, 60 fps target).
2. Provide a block editor (Blockly) for non-programmers.
3. Show stats, genealogy, and replay views.
4. Communicate with the backend via REST + WebSocket.

## Decision

### SvelteKit 2 + Svelte 5

- Svelte's compiled reactivity model eliminates virtual-DOM overhead, which matters when the world view updates every tick.
- SvelteKit gives file-based routing and SSR for public pages (world viewer, stats) out of the box.
- Small bundle size compared to React/Next.js or Vue/Nuxt.
- Svelte 5 runes provide fine-grained reactivity without boilerplate.

### Canvas2D (not WebGL)

- The world is a 2D grid with simple colored cells and sprites. Canvas2D handles this with trivial complexity.
- WebGL would add shader compilation, context management, and a larger learning curve for contributors, with no visible benefit at our scale (grids up to 256x256).
- If performance becomes a bottleneck (e.g., 1000x1000 grids, particle effects), we can migrate the renderer to WebGL without touching the rest of the frontend.

### Token storage: httpOnly cookies

- Initially used localStorage (quick prototype). Migrated to httpOnly cookies to close XSS token theft vector.
- Backend sets `Set-Cookie` with `httpOnly`, `Secure`, `SameSite=Lax`.
- Frontend uses `credentials: 'include'` on all fetch calls.

### Vitest for testing

- Vitest is the natural test runner for Vite-based projects (SvelteKit uses Vite).
- Fast, compatible with Svelte component testing via `@testing-library/svelte`.

## Consequences

- Contributors need to learn Svelte (smaller ecosystem than React, but simpler API).
- Canvas2D caps our visual complexity; acceptable for v1.
- httpOnly cookies require CORS configuration (`credentials: 'include'` + `Access-Control-Allow-Credentials`).
