"""AST node types for GoL Script."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True, slots=True)
class IntLit:
    value: int
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class BoolLit:
    value: bool
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class Var:
    name: str
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class BinaryOp:
    op: str
    left: Expr
    right: Expr
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class UnaryOp:
    op: str
    operand: Expr
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class Call:
    name: str
    args: tuple[Expr, ...]
    line: int
    col: int


# Expression = union of all expression node types
Expr = IntLit | BoolLit | Var | BinaryOp | UnaryOp | Call


@dataclass(frozen=True, slots=True)
class VarDecl:
    name: str
    init: Expr
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class Assignment:
    name: str
    value: Expr
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class ExprStmt:
    expr: Expr
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class IfStmt:
    condition: Expr
    then_block: tuple[Stmt, ...]
    else_block: tuple[Stmt, ...] | None
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class WhileStmt:
    condition: Expr
    body: tuple[Stmt, ...]
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class ReturnStmt:
    value: Expr | None
    line: int
    col: int


# Statement = union of all statement node types
Stmt = VarDecl | Assignment | ExprStmt | IfStmt | WhileStmt | ReturnStmt


@dataclass(frozen=True, slots=True)
class FunctionDef:
    name: str
    params: tuple[str, ...]
    body: tuple[Stmt, ...]
    line: int
    col: int


@dataclass(frozen=True, slots=True)
class Program:
    functions: tuple[FunctionDef, ...]
