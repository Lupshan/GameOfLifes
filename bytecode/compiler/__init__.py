"""GoL Script compiler — public API."""

from __future__ import annotations

from dataclasses import dataclass

from compiler.codegen import CodegenError, generate
from compiler.emit import emit_gob
from compiler.lexer import LexError
from compiler.parser import ParseError, parse
from compiler.sema import SemaError, analyze


@dataclass(frozen=True, slots=True)
class CompileError:
    message: str
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class CompileResult:
    ok: bool
    bytecode: bytes | None = None
    errors: tuple[CompileError, ...] = ()


def compile(source: str) -> CompileResult:  # noqa: A001
    """Compile GoL Script source to bytecode. Returns CompileResult."""
    try:
        prog = parse(source)
        sema = analyze(prog)
        code, local_count = generate(prog, sema)
        gob = emit_gob(code, local_count)
        return CompileResult(ok=True, bytecode=gob)
    except (LexError, ParseError, SemaError) as e:
        line = getattr(e, "line", 0)
        col = getattr(e, "col", 0)
        return CompileResult(ok=False, errors=(CompileError(str(e), line, col),))
    except CodegenError as e:
        return CompileResult(ok=False, errors=(CompileError(str(e), 0, 0),))
