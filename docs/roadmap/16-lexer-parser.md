# 16 — Lexer + parser

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Build the front half of the compiler: source code → AST. Pure Python, well-tested, no codegen yet.

## Scope
- `bytecode/compiler/lexer.py` — tokenizer producing a stream of `Token { kind, value, line, col }`.
- `bytecode/compiler/parser.py` — recursive descent parser producing `bytecode/compiler/ast.py` node types.
- AST nodes: `Program`, `FunctionDef`, `If`, `While`, `Return`, `Assign`, `BinaryOp`, `UnaryOp`, `Call`, `Var`, `IntLit`, `BoolLit`.
- Error reporting with line/col info.
- pytest tests: all sample programs from spec parse correctly; deliberately broken inputs fail with useful errors.

## Non-goals
- No semantic analysis / type checking yet (folded into step 17).
- No language-server-grade error recovery — first error wins.
- No optimization passes.

## Technical notes
- Hand-written, no parser generator. The grammar is small enough.
- AST nodes are `dataclass(frozen=True, slots=True)` for cheap equality + immutability.
- Strict pyright mode for `bytecode/compiler/`.

## Definition of Done
- [ ] Lexer + parser implemented.
- [ ] All sample programs round-trip source → AST.
- [ ] Error tests cover: unexpected token, EOF, unknown identifier.
- [ ] `just test-bytecode` green.
- [ ] `docs/roadmap/README.md` table updated to `done`.
