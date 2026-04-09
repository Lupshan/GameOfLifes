"""GoL Script parser — recursive descent, produces AST from token stream."""

from __future__ import annotations

from compiler.ast_nodes import (
    Assignment,
    BinaryOp,
    BoolLit,
    Call,
    Expr,
    ExprStmt,
    FunctionDef,
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
from compiler.lexer import Token, TokenKind


class ParseError(Exception):
    def __init__(self, message: str, line: int, col: int) -> None:
        super().__init__(f"[{line}:{col}] {message}")
        self.line = line
        self.col = col


class Parser:
    def __init__(self, tokens: list[Token]) -> None:
        self._tokens = tokens
        self._pos = 0

    def _peek(self) -> Token:
        return self._tokens[self._pos]

    def _advance(self) -> Token:
        tok = self._tokens[self._pos]
        self._pos += 1
        return tok

    def _expect(self, kind: TokenKind) -> Token:
        tok = self._peek()
        if tok.kind != kind:
            raise ParseError(
                f"expected {kind.name}, got {tok.kind.name} '{tok.value}'",
                tok.line,
                tok.col,
            )
        return self._advance()

    def _match(self, kind: TokenKind) -> Token | None:
        if self._peek().kind == kind:
            return self._advance()
        return None

    # ---- Top-level ----

    def parse(self) -> Program:
        functions: list[FunctionDef] = []
        while self._peek().kind != TokenKind.EOF:
            functions.append(self._function_def())
        return Program(functions=tuple(functions))

    def _function_def(self) -> FunctionDef:
        tok = self._expect(TokenKind.FN)
        name_tok = self._expect(TokenKind.IDENT)
        self._expect(TokenKind.LPAREN)
        params: list[str] = []
        if self._peek().kind != TokenKind.RPAREN:
            params.append(self._expect(TokenKind.IDENT).value)
            while self._match(TokenKind.COMMA):
                params.append(self._expect(TokenKind.IDENT).value)
        self._expect(TokenKind.RPAREN)
        body = self._block()
        return FunctionDef(
            name=name_tok.value,
            params=tuple(params),
            body=body,
            line=tok.line,
            col=tok.col,
        )

    def _block(self) -> tuple[Stmt, ...]:
        self._expect(TokenKind.LBRACE)
        stmts: list[Stmt] = []
        while self._peek().kind != TokenKind.RBRACE:
            stmts.append(self._statement())
        self._expect(TokenKind.RBRACE)
        return tuple(stmts)

    # ---- Statements ----

    def _statement(self) -> Stmt:
        tok = self._peek()

        if tok.kind == TokenKind.LET:
            return self._var_decl()
        if tok.kind == TokenKind.IF:
            return self._if_stmt()
        if tok.kind == TokenKind.WHILE:
            return self._while_stmt()
        if tok.kind == TokenKind.RETURN:
            return self._return_stmt()

        # Assignment or expression statement.
        # Lookahead: IDENT followed by '=' (but not '==').
        if tok.kind == TokenKind.IDENT and self._is_assignment():
            return self._assignment()

        return self._expr_stmt()

    def _is_assignment(self) -> bool:
        """Check if current position is an assignment (IDENT = ..., not IDENT ==)."""
        if self._pos + 1 >= len(self._tokens):
            return False
        return self._tokens[self._pos + 1].kind == TokenKind.EQ and (
            self._pos + 2 >= len(self._tokens) or self._tokens[self._pos + 2].kind != TokenKind.EQ
        )

    def _var_decl(self) -> VarDecl:
        tok = self._expect(TokenKind.LET)
        name = self._expect(TokenKind.IDENT).value
        self._expect(TokenKind.EQ)
        init = self._expression()
        self._expect(TokenKind.SEMICOLON)
        return VarDecl(name=name, init=init, line=tok.line, col=tok.col)

    def _assignment(self) -> Assignment:
        name_tok = self._expect(TokenKind.IDENT)
        self._expect(TokenKind.EQ)
        value = self._expression()
        self._expect(TokenKind.SEMICOLON)
        return Assignment(name=name_tok.value, value=value, line=name_tok.line, col=name_tok.col)

    def _if_stmt(self) -> IfStmt:
        tok = self._expect(TokenKind.IF)
        self._expect(TokenKind.LPAREN)
        cond = self._expression()
        self._expect(TokenKind.RPAREN)
        then_block = self._block()
        else_block: tuple[Stmt, ...] | None = None
        if self._match(TokenKind.ELSE):
            else_block = self._block()
        return IfStmt(
            condition=cond,
            then_block=then_block,
            else_block=else_block,
            line=tok.line,
            col=tok.col,
        )

    def _while_stmt(self) -> WhileStmt:
        tok = self._expect(TokenKind.WHILE)
        self._expect(TokenKind.LPAREN)
        cond = self._expression()
        self._expect(TokenKind.RPAREN)
        body = self._block()
        return WhileStmt(condition=cond, body=body, line=tok.line, col=tok.col)

    def _return_stmt(self) -> ReturnStmt:
        tok = self._expect(TokenKind.RETURN)
        value: Expr | None = None
        if self._peek().kind != TokenKind.SEMICOLON:
            value = self._expression()
        self._expect(TokenKind.SEMICOLON)
        return ReturnStmt(value=value, line=tok.line, col=tok.col)

    def _expr_stmt(self) -> ExprStmt:
        expr = self._expression()
        self._expect(TokenKind.SEMICOLON)
        return ExprStmt(expr=expr, line=expr.line, col=expr.col)

    # ---- Expressions (precedence climbing) ----

    def _expression(self) -> Expr:
        return self._or_expr()

    def _or_expr(self) -> Expr:
        left = self._and_expr()
        while self._peek().kind == TokenKind.PIPEPIPE:
            op_tok = self._advance()
            right = self._and_expr()
            left = BinaryOp("||", left, right, op_tok.line, op_tok.col)
        return left

    def _and_expr(self) -> Expr:
        left = self._equality()
        while self._peek().kind == TokenKind.AMPAMP:
            op_tok = self._advance()
            right = self._equality()
            left = BinaryOp("&&", left, right, op_tok.line, op_tok.col)
        return left

    def _equality(self) -> Expr:
        left = self._comparison()
        while self._peek().kind in (TokenKind.EQEQ, TokenKind.BANGEQ):
            op_tok = self._advance()
            right = self._comparison()
            left = BinaryOp(op_tok.value, left, right, op_tok.line, op_tok.col)
        return left

    def _comparison(self) -> Expr:
        left = self._addition()
        while self._peek().kind in (TokenKind.LT, TokenKind.GT, TokenKind.LTEQ, TokenKind.GTEQ):
            op_tok = self._advance()
            right = self._addition()
            left = BinaryOp(op_tok.value, left, right, op_tok.line, op_tok.col)
        return left

    def _addition(self) -> Expr:
        left = self._multiplication()
        while self._peek().kind in (TokenKind.PLUS, TokenKind.MINUS):
            op_tok = self._advance()
            right = self._multiplication()
            left = BinaryOp(op_tok.value, left, right, op_tok.line, op_tok.col)
        return left

    def _multiplication(self) -> Expr:
        left = self._unary()
        while self._peek().kind in (TokenKind.STAR, TokenKind.SLASH, TokenKind.PERCENT):
            op_tok = self._advance()
            right = self._unary()
            left = BinaryOp(op_tok.value, left, right, op_tok.line, op_tok.col)
        return left

    def _unary(self) -> Expr:
        if self._peek().kind in (TokenKind.MINUS, TokenKind.BANG):
            op_tok = self._advance()
            operand = self._unary()
            return UnaryOp(op_tok.value, operand, op_tok.line, op_tok.col)
        return self._primary()

    def _primary(self) -> Expr:
        tok = self._peek()

        if tok.kind == TokenKind.INT_LIT:
            self._advance()
            return IntLit(int(tok.value), tok.line, tok.col)

        if tok.kind == TokenKind.TRUE:
            self._advance()
            return BoolLit(True, tok.line, tok.col)

        if tok.kind == TokenKind.FALSE:
            self._advance()
            return BoolLit(False, tok.line, tok.col)

        if tok.kind == TokenKind.IDENT:
            self._advance()
            # Check for function call.
            if self._peek().kind == TokenKind.LPAREN:
                self._advance()  # consume '('
                args: list[Expr] = []
                if self._peek().kind != TokenKind.RPAREN:
                    args.append(self._expression())
                    while self._match(TokenKind.COMMA):
                        args.append(self._expression())
                self._expect(TokenKind.RPAREN)
                return Call(name=tok.value, args=tuple(args), line=tok.line, col=tok.col)
            return Var(name=tok.value, line=tok.line, col=tok.col)

        if tok.kind == TokenKind.LPAREN:
            self._advance()
            expr = self._expression()
            self._expect(TokenKind.RPAREN)
            return expr

        raise ParseError(
            f"unexpected token {tok.kind.name} '{tok.value}'",
            tok.line,
            tok.col,
        )


def parse(source: str) -> Program:
    """Convenience: tokenize + parse in one call."""
    from compiler.lexer import tokenize

    tokens = tokenize(source)
    return Parser(tokens).parse()
