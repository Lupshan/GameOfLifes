"""Tests for the GoL Script codegen pipeline (parse → sema → codegen → emit)."""

from __future__ import annotations

import struct

import pytest

from compiler.codegen import generate
from compiler.emit import MAGIC, VERSION, emit_gob
from compiler.parser import parse
from compiler.sema import SemaError, analyze


def compile_source(source: str) -> bytes:
    """Full pipeline: source → .gob bytes."""
    prog = parse(source)
    sema = analyze(prog)
    code, local_count = generate(prog, sema)
    return emit_gob(code, local_count)


def compile_to_code(source: str) -> bytes:
    """Pipeline: source → raw code bytes (no .gob header)."""
    prog = parse(source)
    sema = analyze(prog)
    code, _ = generate(prog, sema)
    return code


# ---- .gob format ----


def test_gob_header() -> None:
    gob = compile_source("fn main() {}")
    assert len(gob) >= 16
    magic, version = struct.unpack_from("<IH", gob, 0)
    assert magic == MAGIC
    assert version == VERSION


# ---- Simple programs compile ----


def test_empty_main() -> None:
    code = compile_to_code("fn main() {}")
    # Should contain CALL, then HALT, then RETURN.
    assert len(code) > 0


def test_int_literal() -> None:
    code = compile_to_code("fn main() { let x = 42; }")
    # Should contain PUSH_INT 42 somewhere.
    assert 0x01 in code  # PUSH_INT opcode


def test_bool_literal() -> None:
    code = compile_to_code("fn main() { let x = true; }")
    assert 0x02 in code  # PUSH_BOOL opcode


def test_binary_add() -> None:
    code = compile_to_code("fn main() { let x = 1 + 2; }")
    assert 0x10 in code  # ADD opcode


def test_unary_neg() -> None:
    code = compile_to_code("fn main() { let x = -42; }")
    assert 0x15 in code  # NEG opcode


def test_if_stmt() -> None:
    code = compile_to_code("fn main() { if (true) { let x = 1; } }")
    assert 0x31 in code  # JMP_IF_FALSE opcode


def test_while_stmt() -> None:
    code = compile_to_code("fn main() { while (false) { let x = 1; } }")
    assert 0x30 in code  # JMP opcode


def test_return_stmt() -> None:
    code = compile_to_code("fn main() { return; }")
    assert 0x33 in code  # RETURN opcode


def test_move_intrinsic() -> None:
    code = compile_to_code("fn main() { move(); }")
    assert 0x82 in code  # MOVE opcode


def test_eat_intrinsic() -> None:
    code = compile_to_code("fn main() { eat(); }")
    assert 0x85 in code  # EAT opcode


def test_perceive_food_sugar() -> None:
    code = compile_to_code("fn main() { let f = perceive_food(); }")
    assert 0x80 in code  # PERCEIVE opcode


def test_my_trait_intrinsic() -> None:
    code = compile_to_code("fn main() { let m = my_trait(2); }")
    assert 0x8A in code  # MY_TRAIT opcode


# ---- Sample programs compile ----


def test_random_walker_compiles() -> None:
    source = """
    fn main() {
        turn_right();
        move();
        eat();
        drink();
    }
    """
    gob = compile_source(source)
    assert len(gob) > 16


def test_food_seeker_compiles() -> None:
    source = """
    fn main() {
        let food = look_food();
        if (food > 0) {
            move();
            eat();
        } else {
            turn_right();
        }
        drink();
        if (my_energy() > 100) {
            reproduce();
        }
    }
    """
    gob = compile_source(source)
    assert len(gob) > 16


def test_opportunistic_eater_compiles() -> None:
    source = """
    fn main() {
        let here_food = perceive_food();
        if (here_food > 0) {
            eat();
        }
        let here_water = perceive_water();
        if (here_water > 0) {
            drink();
        }
        let ahead_food = look_food();
        if (ahead_food > 0) {
            move();
        } else {
            if (my_energy() % 2 == 0) {
                turn_left();
            } else {
                turn_right();
            }
        }
        if (my_energy() > 150 && my_hydration() > 50) {
            reproduce();
        }
    }
    """
    gob = compile_source(source)
    assert len(gob) > 16


# ---- Sema errors ----


def test_sema_undefined_function() -> None:
    with pytest.raises(SemaError, match="undefined function"):
        prog = parse("fn main() { foo(); }")
        analyze(prog)


def test_sema_arity_mismatch() -> None:
    with pytest.raises(SemaError, match="expects 0 args"):
        prog = parse("fn main() { move(1); }")
        analyze(prog)


def test_sema_duplicate_function() -> None:
    with pytest.raises(SemaError, match="duplicate function"):
        prog = parse("fn main() {} fn main() {}")
        analyze(prog)


# ---- Comparison operators ----


def test_not_equal() -> None:
    code = compile_to_code("fn main() { let x = 1 != 2; }")
    assert 0x20 in code  # EQ
    assert 0x25 in code  # NOT


def test_less_equal() -> None:
    code = compile_to_code("fn main() { let x = 1 <= 2; }")
    assert 0x22 in code  # GT
    assert 0x25 in code  # NOT


def test_greater_equal() -> None:
    code = compile_to_code("fn main() { let x = 1 >= 2; }")
    assert 0x21 in code  # LT
    assert 0x25 in code  # NOT
