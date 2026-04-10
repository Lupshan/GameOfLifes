# ADR 013 — Blockly for visual block editor

**Status**: accepted
**Date**: 2026-04-10

## Context

The Tier 1 entry point for non-coders needs a visual block editor. Blockly is the only mature library for this purpose (others: Scratch-blocks is Blockly-based, Snap! is monolithic).

## Decision

**Use Blockly** despite its Google origin. The project is now community-governed under the Blockly GitHub organization, MIT licensed, with independent maintainers. There is no viable alternative for visual block editing.

- Custom block definitions in JSON for GoL Script constructs (actions, perception, control flow).
- Blockly's code generator API adapted to emit GoL Script source text.
- Dynamic import in SvelteKit (`ssr: false` for the `/edit` route) since Blockly requires DOM access.

## Consequences

- ~300KB additional bundle size for the `/edit` route only (tree-shaken, not loaded on other pages).
- Blockly's API is stable and well-documented; custom blocks are straightforward JSON definitions.
- The generated GoL Script source is compiled by the existing Python compiler — no changes needed to the compilation pipeline.
- If Blockly governance changes unfavorably, the block definitions and generator are our code; only the Blockly runtime would need replacing.
