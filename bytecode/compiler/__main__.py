"""GoL Script compiler CLI.

Usage: python -m compiler <input.gol> [-o <output.gob>]
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from compiler.codegen import generate
from compiler.emit import emit_gob
from compiler.lexer import LexError
from compiler.parser import ParseError, parse
from compiler.sema import SemaError, analyze


def main() -> None:
    parser = argparse.ArgumentParser(description="GoL Script compiler")
    parser.add_argument("input", help="Source file (.gol)")
    parser.add_argument("-o", "--output", help="Output file (.gob)", default=None)
    args = parser.parse_args()

    input_path = Path(args.input)
    if not input_path.exists():
        print(f"error: file not found: {input_path}", file=sys.stderr)
        sys.exit(1)

    source = input_path.read_text(encoding="utf-8")

    try:
        prog = parse(source)
        sema = analyze(prog)
        code, local_count = generate(prog, sema)
        gob = emit_gob(code, local_count)
    except (LexError, ParseError, SemaError) as e:
        print(f"error: {e}", file=sys.stderr)
        sys.exit(1)

    output_path = Path(args.output) if args.output else input_path.with_suffix(".gob")
    output_path.write_bytes(gob)
    print(f"compiled {input_path} → {output_path} ({len(gob)} bytes)")


if __name__ == "__main__":
    main()
