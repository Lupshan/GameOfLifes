"""Tests for the GoL Script parser."""

import pytest

from compiler.ast_nodes import (
    Assignment,
    BinaryOp,
    BoolLit,
    Call,
    ExprStmt,
    IfStmt,
    IntLit,
    ReturnStmt,
    UnaryOp,
    Var,
    VarDecl,
    WhileStmt,
)
from compiler.parser import ParseError, parse


def test_empty_program() -> None:
    prog = parse("")
    assert len(prog.functions) == 0


def test_minimal_function() -> None:
    prog = parse("fn main() {}")
    assert len(prog.functions) == 1
    assert prog.functions[0].name == "main"
    assert prog.functions[0].params == ()
    assert prog.functions[0].body == ()


def test_function_with_params() -> None:
    prog = parse("fn add(a, b) { return a; }")
    f = prog.functions[0]
    assert f.params == ("a", "b")
    assert len(f.body) == 1
    assert isinstance(f.body[0], ReturnStmt)


def test_var_decl() -> None:
    prog = parse("fn main() { let x = 42; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    assert stmt.name == "x"
    assert isinstance(stmt.init, IntLit)
    assert stmt.init.value == 42


def test_assignment() -> None:
    prog = parse("fn main() { let x = 1; x = 2; }")
    stmt = prog.functions[0].body[1]
    assert isinstance(stmt, Assignment)
    assert stmt.name == "x"
    assert isinstance(stmt.value, IntLit)
    assert stmt.value.value == 2


def test_if_stmt() -> None:
    prog = parse("fn main() { if (true) { return; } }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, IfStmt)
    assert isinstance(stmt.condition, BoolLit)
    assert stmt.else_block is None


def test_if_else() -> None:
    prog = parse("fn main() { if (false) { return; } else { return; } }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, IfStmt)
    assert stmt.else_block is not None


def test_while_stmt() -> None:
    prog = parse("fn main() { while (true) { move(); } }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, WhileStmt)
    assert isinstance(stmt.condition, BoolLit)


def test_return_value() -> None:
    prog = parse("fn f() { return 42; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, ReturnStmt)
    assert isinstance(stmt.value, IntLit)


def test_return_void() -> None:
    prog = parse("fn f() { return; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, ReturnStmt)
    assert stmt.value is None


def test_function_call() -> None:
    prog = parse("fn main() { eat(); }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, ExprStmt)
    assert isinstance(stmt.expr, Call)
    assert stmt.expr.name == "eat"
    assert stmt.expr.args == ()


def test_function_call_with_args() -> None:
    prog = parse("fn main() { my_trait(2); }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, ExprStmt)
    call = stmt.expr
    assert isinstance(call, Call)
    assert len(call.args) == 1
    assert isinstance(call.args[0], IntLit)


def test_binary_operators() -> None:
    prog = parse("fn main() { let x = 1 + 2 * 3; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    # Should be: 1 + (2 * 3) due to precedence.
    expr = stmt.init
    assert isinstance(expr, BinaryOp)
    assert expr.op == "+"
    assert isinstance(expr.left, IntLit)
    assert isinstance(expr.right, BinaryOp)
    assert expr.right.op == "*"


def test_unary_neg() -> None:
    prog = parse("fn main() { let x = -42; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    assert isinstance(stmt.init, UnaryOp)
    assert stmt.init.op == "-"


def test_unary_not() -> None:
    prog = parse("fn main() { let x = !true; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    assert isinstance(stmt.init, UnaryOp)
    assert stmt.init.op == "!"


def test_comparison_chain() -> None:
    prog = parse("fn main() { let x = 1 < 2 && 3 > 1; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    assert isinstance(stmt.init, BinaryOp)
    assert stmt.init.op == "&&"


def test_parenthesized_expr() -> None:
    prog = parse("fn main() { let x = (1 + 2) * 3; }")
    stmt = prog.functions[0].body[0]
    assert isinstance(stmt, VarDecl)
    expr = stmt.init
    assert isinstance(expr, BinaryOp)
    assert expr.op == "*"
    assert isinstance(expr.left, BinaryOp)
    assert expr.left.op == "+"


def test_variable_reference() -> None:
    prog = parse("fn main() { let x = 1; let y = x; }")
    stmt = prog.functions[0].body[1]
    assert isinstance(stmt, VarDecl)
    assert isinstance(stmt.init, Var)
    assert stmt.init.name == "x"


# ---- Sample programs from spec ----


def test_random_walker() -> None:
    source = """
    fn main() {
        turn_right();
        move();
        eat();
        drink();
    }
    """
    prog = parse(source)
    assert len(prog.functions) == 1
    assert len(prog.functions[0].body) == 4


def test_food_seeker() -> None:
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
    prog = parse(source)
    assert len(prog.functions) == 1
    f = prog.functions[0]
    assert len(f.body) == 4  # let, if, drink, if


def test_opportunistic_eater() -> None:
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
    prog = parse(source)
    assert len(prog.functions) == 1


def test_multiple_functions() -> None:
    source = """
    fn should_eat() {
        return my_energy() < 50;
    }
    fn main() {
        if (should_eat()) {
            eat();
        }
    }
    """
    prog = parse(source)
    assert len(prog.functions) == 2
    assert prog.functions[0].name == "should_eat"
    assert prog.functions[1].name == "main"


# ---- Error cases ----


def test_error_unexpected_token() -> None:
    with pytest.raises(ParseError):
        parse("fn main() { + }")


def test_error_missing_semicolon() -> None:
    with pytest.raises(ParseError):
        parse("fn main() { let x = 1 }")


def test_error_unexpected_eof() -> None:
    with pytest.raises(ParseError):
        parse("fn main() {")


def test_error_missing_paren() -> None:
    with pytest.raises(ParseError):
        parse("fn main( {}")
