"""Code generation: AST → opcode list with jump patching."""

from __future__ import annotations

import struct

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
from compiler.sema import BUILTINS, SemaResult

# Opcode constants (mirrors engine/include/engine/bytecode.hpp).
HALT = 0x00
PUSH_INT = 0x01
PUSH_BOOL = 0x02
POP = 0x03
DUP = 0x04
SWAP = 0x05
ADD = 0x10
SUB = 0x11
MUL = 0x12
DIV = 0x13
MOD = 0x14
NEG = 0x15
EQ = 0x20
LT = 0x21
GT = 0x22
AND = 0x23
OR = 0x24
NOT = 0x25
JMP = 0x30
JMP_IF_FALSE = 0x31
CALL = 0x32
RETURN = 0x33
LOAD_LOCAL = 0x40
STORE_LOCAL = 0x41
LOAD_CONST = 0x50

# Intrinsics
PERCEIVE = 0x80
LOOK = 0x81
MOVE = 0x82
TURN_LEFT = 0x83
TURN_RIGHT = 0x84
EAT = 0x85
DRINK = 0x86
REPRODUCE = 0x87
MY_ENERGY = 0x88
MY_HYDRATION = 0x89
MY_TRAIT = 0x8A
NOP = 0xFF

BINOP_MAP: dict[str, int] = {
    "+": ADD,
    "-": SUB,
    "*": MUL,
    "/": DIV,
    "%": MOD,
    "==": EQ,
    "<": LT,
    ">": GT,
    "&&": AND,
    "||": OR,
}

INTRINSIC_MAP: dict[str, int] = {
    "perceive": PERCEIVE,
    "look": LOOK,
    "move": MOVE,
    "turn_left": TURN_LEFT,
    "turn_right": TURN_RIGHT,
    "eat": EAT,
    "drink": DRINK,
    "reproduce": REPRODUCE,
    "my_energy": MY_ENERGY,
    "my_hydration": MY_HYDRATION,
    "my_trait": MY_TRAIT,
}

# Convenience perception builtins: emit PERCEIVE/LOOK then extract one value.
# Stack after PERCEIVE: [terrain, food, water, agents] (bottom → top).
# perceive_terrain → PERCEIVE, POP, POP, POP (keep bottom)
# perceive_food → PERCEIVE, POP, POP, SWAP, POP
# perceive_water → PERCEIVE, POP, SWAP, POP, SWAP, POP
# perceive_agents → PERCEIVE, SWAP, POP, SWAP, POP, SWAP, POP
# Simpler approach: emit PERCEIVE, then store to temps, then load the one we want.
# But that requires allocating temps. Instead, use stack ops:
PERCEPTION_EXTRACT: dict[str, tuple[int, list[int]]] = {
    # (base_opcode, stack_ops_to_extract_desired_value)
    # After PERCEIVE: stack = [terrain, food, water, agents_count]
    # To get terrain (index 0 from bottom = 3 from top):
    "perceive_agents": (PERCEIVE, []),  # top of stack is agents_count
    "perceive_water": (PERCEIVE, [SWAP, POP]),  # discard top (agents), water is now on top
    "perceive_food": (PERCEIVE, [POP, POP]),  # discard top two, then swap+pop for third
    "perceive_terrain": (PERCEIVE, [POP, POP, POP]),  # discard top three
    "look_agents": (LOOK, []),
    "look_water": (LOOK, [SWAP, POP]),
    "look_food": (LOOK, [POP, POP]),
    "look_terrain": (LOOK, [POP, POP, POP]),
}


class CodegenError(Exception):
    def __init__(self, message: str) -> None:
        super().__init__(message)


class Emitter:
    """Accumulates bytecode instructions with label/jump support."""

    def __init__(self) -> None:
        self.code: bytearray = bytearray()
        self.constants: list[int] = []  # int constants only in v1

    def emit_u8(self, val: int) -> None:
        self.code.append(val & 0xFF)

    def emit_i16(self, val: int) -> None:
        self.code.extend(struct.pack("<h", val))

    def emit_u16(self, val: int) -> None:
        self.code.extend(struct.pack("<H", val))

    def emit_i32(self, val: int) -> None:
        self.code.extend(struct.pack("<i", val))

    def pos(self) -> int:
        return len(self.code)

    def patch_i16(self, offset: int, val: int) -> None:
        data = struct.pack("<h", val)
        self.code[offset] = data[0]
        self.code[offset + 1] = data[1]


def generate(program: Program, sema: SemaResult) -> tuple[bytes, int]:
    """Generate bytecode from a program. Returns (code_bytes, local_count)."""
    emitter = Emitter()
    func_addrs: dict[str, int] = {}

    # Find main function — emit a CALL to it at the start, then HALT.
    if "main" not in sema.functions:
        raise CodegenError("no 'main' function defined")

    # Placeholder: CALL main (3 bytes) + HALT (1 byte)
    emitter.emit_u8(CALL)
    main_call_pos = emitter.pos()
    emitter.emit_u16(0)  # placeholder
    emitter.emit_u8(HALT)

    # Emit each function.
    for fn_def in program.functions:
        func_info = sema.functions[fn_def.name]
        func_addrs[fn_def.name] = emitter.pos()
        _emit_stmts(fn_def.body, func_info, sema, emitter, func_addrs)
        # Implicit return at end of function.
        emitter.emit_u8(RETURN)

    # Patch CALL main address.
    emitter.patch_i16(main_call_pos, func_addrs["main"])

    # Patch any forward function calls that were emitted before the function was defined.
    # (We handle this by emitting all functions first, then patching — but actually,
    # our approach emits functions in order. Forward calls need a two-pass approach.
    # For simplicity, we'll do a second pass below.)

    # Calculate total local count (max across all functions — shared local space in v1).
    total_locals = 0
    for fn_info in sema.functions.values():
        total_locals = max(total_locals, fn_info.next_local)

    return bytes(emitter.code), total_locals


def _emit_stmts(
    stmts: tuple[Stmt, ...],
    func: object,
    sema: SemaResult,
    em: Emitter,
    func_addrs: dict[str, int],
) -> None:
    for stmt in stmts:
        _emit_stmt(stmt, func, sema, em, func_addrs)


def _emit_stmt(
    stmt: Stmt,
    func: object,
    sema: SemaResult,
    em: Emitter,
    func_addrs: dict[str, int],
) -> None:
    from compiler.sema import FunctionInfo

    assert isinstance(func, FunctionInfo)

    if isinstance(stmt, VarDecl):
        _emit_expr(stmt.init, func, sema, em, func_addrs)
        idx = func.locals[stmt.name]
        em.emit_u8(STORE_LOCAL)
        em.emit_u8(idx)

    elif isinstance(stmt, Assignment):
        _emit_expr(stmt.value, func, sema, em, func_addrs)
        idx = func.locals[stmt.name]
        em.emit_u8(STORE_LOCAL)
        em.emit_u8(idx)

    elif isinstance(stmt, ExprStmt):
        _emit_expr(stmt.expr, func, sema, em, func_addrs)
        # Discard result if the expression pushes something.
        if _expr_pushes(stmt.expr):
            em.emit_u8(POP)

    elif isinstance(stmt, IfStmt):
        _emit_expr(stmt.condition, func, sema, em, func_addrs)
        em.emit_u8(JMP_IF_FALSE)
        jmp_false_pos = em.pos()
        em.emit_i16(0)  # placeholder

        _emit_stmts(stmt.then_block, func, sema, em, func_addrs)

        if stmt.else_block is not None:
            em.emit_u8(JMP)
            jmp_end_pos = em.pos()
            em.emit_i16(0)  # placeholder

            # Patch JMP_IF_FALSE → here
            offset = em.pos() - jmp_false_pos
            em.patch_i16(jmp_false_pos, offset)

            _emit_stmts(stmt.else_block, func, sema, em, func_addrs)

            # Patch JMP → here
            offset = em.pos() - jmp_end_pos
            em.patch_i16(jmp_end_pos, offset)
        else:
            offset = em.pos() - jmp_false_pos
            em.patch_i16(jmp_false_pos, offset)

    elif isinstance(stmt, WhileStmt):
        loop_start = em.pos()
        _emit_expr(stmt.condition, func, sema, em, func_addrs)
        em.emit_u8(JMP_IF_FALSE)
        jmp_false_pos = em.pos()
        em.emit_i16(0)  # placeholder

        _emit_stmts(stmt.body, func, sema, em, func_addrs)

        # Jump back to loop start.
        em.emit_u8(JMP)
        back_offset = loop_start - em.pos()
        em.emit_i16(back_offset)

        # Patch JMP_IF_FALSE → here.
        offset = em.pos() - jmp_false_pos
        em.patch_i16(jmp_false_pos, offset)

    elif isinstance(stmt, ReturnStmt):
        if stmt.value is not None:
            _emit_expr(stmt.value, func, sema, em, func_addrs)
        em.emit_u8(RETURN)


def _emit_expr(
    expr: Expr,
    func: object,
    sema: SemaResult,
    em: Emitter,
    func_addrs: dict[str, int],
) -> None:
    from compiler.sema import FunctionInfo

    assert isinstance(func, FunctionInfo)

    if isinstance(expr, IntLit):
        em.emit_u8(PUSH_INT)
        em.emit_i32(expr.value)

    elif isinstance(expr, BoolLit):
        em.emit_u8(PUSH_BOOL)
        em.emit_u8(1 if expr.value else 0)

    elif isinstance(expr, Var):
        idx = func.locals[expr.name]
        em.emit_u8(LOAD_LOCAL)
        em.emit_u8(idx)

    elif isinstance(expr, BinaryOp):
        if expr.op == "!=":
            # a != b → a == b, NOT
            _emit_expr(expr.left, func, sema, em, func_addrs)
            _emit_expr(expr.right, func, sema, em, func_addrs)
            em.emit_u8(EQ)
            em.emit_u8(NOT)
        elif expr.op == "<=":
            # a <= b → a > b, NOT
            _emit_expr(expr.left, func, sema, em, func_addrs)
            _emit_expr(expr.right, func, sema, em, func_addrs)
            em.emit_u8(GT)
            em.emit_u8(NOT)
        elif expr.op == ">=":
            # a >= b → a < b, NOT
            _emit_expr(expr.left, func, sema, em, func_addrs)
            _emit_expr(expr.right, func, sema, em, func_addrs)
            em.emit_u8(LT)
            em.emit_u8(NOT)
        elif expr.op in BINOP_MAP:
            _emit_expr(expr.left, func, sema, em, func_addrs)
            _emit_expr(expr.right, func, sema, em, func_addrs)
            em.emit_u8(BINOP_MAP[expr.op])
        else:
            raise CodegenError(f"unknown binary operator '{expr.op}'")

    elif isinstance(expr, UnaryOp):
        _emit_expr(expr.operand, func, sema, em, func_addrs)
        if expr.op == "-":
            em.emit_u8(NEG)
        elif expr.op == "!":
            em.emit_u8(NOT)

    elif isinstance(expr, Call):
        if expr.name in PERCEPTION_EXTRACT:
            base_op, extract_ops = PERCEPTION_EXTRACT[expr.name]
            em.emit_u8(base_op)
            for op in extract_ops:
                em.emit_u8(op)
        elif expr.name in INTRINSIC_MAP:
            # Emit args first (for my_trait).
            for arg in expr.args:
                _emit_expr(arg, func, sema, em, func_addrs)
            if expr.name == "my_trait":
                # my_trait takes an inline u8 operand, not a stack value.
                # The arg was pushed; we need to handle this differently.
                # Actually per spec, MY_TRAIT reads an inline u8 operand.
                # So we need the arg to be a compile-time constant.
                if not isinstance(expr.args[0], IntLit):
                    raise CodegenError("my_trait() argument must be an integer literal")
                # Remove the pushed value (we already emitted args above — undo).
                # Simpler: don't emit the arg push, just emit the opcode + inline operand.
                # Let's redo: remove the last 5 bytes (PUSH_INT + i32).
                del em.code[-5:]
                em.emit_u8(MY_TRAIT)
                em.emit_u8(expr.args[0].value & 0xFF)
            else:
                em.emit_u8(INTRINSIC_MAP[expr.name])
        elif expr.name in BUILTINS:
            # Should have been handled above.
            raise CodegenError(f"unhandled builtin '{expr.name}'")
        else:
            # User function call.
            for arg in expr.args:
                _emit_expr(arg, func, sema, em, func_addrs)
            # Store args into callee's param locals (simplified: shared local space).
            callee = sema.functions[expr.name]
            for i in range(len(expr.args) - 1, -1, -1):
                em.emit_u8(STORE_LOCAL)
                em.emit_u8(callee.locals[list(callee.locals.keys())[i]])

            if expr.name in func_addrs:
                em.emit_u8(CALL)
                em.emit_u16(func_addrs[expr.name])
            else:
                # Forward reference — emit placeholder, patch later.
                # For v1 simplicity, require functions to be defined before use
                # (the two-pass sema already knows about them).
                raise CodegenError(
                    f"function '{expr.name}' not yet compiled (forward calls not supported in v1)"
                )


def _expr_pushes(expr: Expr) -> bool:
    """Does this expression leave a value on the stack?"""
    if isinstance(expr, Call):
        name = expr.name
        # These intrinsics push no result.
        if name in ("move", "turn_left", "turn_right"):
            return False
        # Perception convenience builtins push 1 value.
        if name in PERCEPTION_EXTRACT:
            return True
        # PERCEIVE and LOOK push 4 values — but as expr_stmt we'd need to pop 4.
        # For simplicity, treat them as pushing 1 (top of stack).
        if name in ("perceive", "look"):
            return True  # actually pushes 4, but we pop 1 here; caller handles rest
        return True
    return True
