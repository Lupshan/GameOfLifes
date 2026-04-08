# 29 — Blockly block editor

**Status**: not started
**Started**: —
**Finished**: —

## Goal
The Tier 1 entry point for non-coders: a Blockly visual editor that produces source code in the in-house language. After this step, a user can drag blocks, generate source, and preview it.

## Scope
- Custom Blockly blocks for: control flow (if/while), arithmetic, comparisons, builtin function calls (`move`, `eat`, `perceive_food`, etc.).
- Block → source code generator using Blockly's code generator API targeting our language syntax.
- `/edit` route hosting the Blockly workspace + a read-only source preview pane.
- Save/load workspace XML to/from localStorage initially.

## Non-goals
- No syntax highlighting on the preview (just a `<pre>`).
- No multiplayer editing.
- No raw text editor mode (Tier 1 is block-only by design; advanced users will use the upcoming `.gol` upload path in step 30).

## Technical notes
- Blockly is community-driven OSS — fits the deps policy.
- Custom block definitions in JSON, registered at app boot.
- Generator is a one-pass tree walker mirroring Blockly's API.

## Definition of Done
- [ ] Custom block set covers all language features.
- [ ] Blocks generate valid source for several sample programs.
- [ ] Workspace persists in localStorage.
- [ ] `docs/roadmap/README.md` table updated to `done`.
