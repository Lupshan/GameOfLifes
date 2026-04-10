"""Semantic analysis: scope resolution, arity checks, local variable allocation."""

from __future__ import annotations

from compiler.ast_nodes import (
    Assignment,
    BinaryOp,
    BoolLit,
    Call,
    Expr,
    ExprStmt,
    IfStmt,
    IntLit,
    Program,
    ReturnStmt,
    Stmt,
    UnaryOp,
    Var,
    VarDecl,
    WhileStmt,
)

# Builtins: name → (arity, is_intrinsic)
BUILTINS: dict[str, int] = {
    "perceive": 0,
    "look": 0,
    "move": 0,
    "turn_left": 0,
    "turn_right": 0,
    "eat": 0,
    "drink": 0,
    "reproduce": 0,
    "my_energy": 0,
    "my_hydration": 0,
    "my_trait": 1,
    # Convenience sugar (compiler-level):
    "perceive_terrain": 0,
    "perceive_food": 0,
    "perceive_water": 0,
    "perceive_agents": 0,
    "look_terrain": 0,
    "look_food": 0,
    "look_water": 0,
    "look_agents": 0,
}


class SemaError(Exception):
    def __init__(self, message: str, line: int, col: int) -> None:
        super().__init__(f"[{line}:{col}] {message}")
        self.line = line
        self.col = col


class FunctionInfo:
    def __init__(self, name: str, arity: int) -> None:
        self.name = name
        self.arity = arity
        self.locals: dict[str, int] = {}  # name → local slot index
        self.next_local: int = 0

    def resolve_or_alloc_local(self, name: str) -> int:
        if name not in self.locals:
            self.locals[name] = self.next_local
            self.next_local += 1
        return self.locals[name]

    def resolve_local(self, name: str) -> int | None:
        return self.locals.get(name)


class SemaResult:
    """Result of semantic analysis: function table + local allocations."""

    def __init__(self) -> None:
        self.functions: dict[str, FunctionInfo] = {}

    def add_function(self, name: str, arity: int) -> FunctionInfo:
        info = FunctionInfo(name, arity)
        self.functions[name] = info
        return info


def analyze(program: Program) -> SemaResult:
    """Run semantic analysis. Raises SemaError on failure."""
    result = SemaResult()

    # First pass: register all functions.
    for fn in program.functions:
        if fn.name in result.functions:
            raise SemaError(f"duplicate function '{fn.name}'", fn.line, fn.col)
        result.add_function(fn.name, len(fn.params))

    # Second pass: check each function body.
    for fn in program.functions:
        info = result.functions[fn.name]
        # Allocate params as locals.
        for param in fn.params:
            info.resolve_or_alloc_local(param)
        _check_stmts(fn.body, info, result)

    return result


def _check_stmts(stmts: tuple[Stmt, ...], func: FunctionInfo, result: SemaResult) -> None:
    for stmt in stmts:
        _check_stmt(stmt, func, result)


def _check_stmt(stmt: Stmt, func: FunctionInfo, result: SemaResult) -> None:
    if isinstance(stmt, VarDecl):
        _check_expr(stmt.init, func, result)
        func.resolve_or_alloc_local(stmt.name)
    elif isinstance(stmt, Assignment):
        _check_expr(stmt.value, func, result)
        # Allow assignment to undeclared variables (they auto-declare).
        func.resolve_or_alloc_local(stmt.name)
    elif isinstance(stmt, ExprStmt):
        _check_expr(stmt.expr, func, result)
    elif isinstance(stmt, IfStmt):
        _check_expr(stmt.condition, func, result)
        _check_stmts(stmt.then_block, func, result)
        if stmt.else_block is not None:
            _check_stmts(stmt.else_block, func, result)
    elif isinstance(stmt, WhileStmt):
        _check_expr(stmt.condition, func, result)
        _check_stmts(stmt.body, func, result)
    else:
        assert isinstance(stmt, ReturnStmt)
        if stmt.value is not None:
            _check_expr(stmt.value, func, result)


def _check_expr(expr: Expr, func: FunctionInfo, result: SemaResult) -> None:
    if isinstance(expr, IntLit | BoolLit):
        pass
    elif isinstance(expr, Var):
        # Variables auto-declare on first use (locals persist across ticks, default to 0).
        func.resolve_or_alloc_local(expr.name)
    elif isinstance(expr, BinaryOp):
        _check_expr(expr.left, func, result)
        _check_expr(expr.right, func, result)
    elif isinstance(expr, UnaryOp):
        _check_expr(expr.operand, func, result)
    else:
        assert isinstance(expr, Call)
        # Check if it's a builtin or user function.
        if expr.name in BUILTINS:
            expected_arity = BUILTINS[expr.name]
            if len(expr.args) != expected_arity:
                raise SemaError(
                    f"builtin '{expr.name}' expects {expected_arity} args, got {len(expr.args)}",
                    expr.line,
                    expr.col,
                )
        elif expr.name in result.functions:
            fn_info = result.functions[expr.name]
            if len(expr.args) != fn_info.arity:
                raise SemaError(
                    f"function '{expr.name}' expects {fn_info.arity} args, got {len(expr.args)}",
                    expr.line,
                    expr.col,
                )
        else:
            raise SemaError(f"undefined function '{expr.name}'", expr.line, expr.col)

        for arg in expr.args:
            _check_expr(arg, func, result)
