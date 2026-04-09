"""GoL Script lexer — tokenizes source code into a stream of tokens."""

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum, auto


class TokenKind(Enum):
    # Literals
    INT_LIT = auto()
    TRUE = auto()
    FALSE = auto()

    # Identifier
    IDENT = auto()

    # Keywords
    FN = auto()
    LET = auto()
    IF = auto()
    ELSE = auto()
    WHILE = auto()
    RETURN = auto()

    # Punctuation
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    SEMICOLON = auto()
    COMMA = auto()

    # Operators
    PLUS = auto()
    MINUS = auto()
    STAR = auto()
    SLASH = auto()
    PERCENT = auto()
    BANG = auto()
    EQ = auto()  # =
    EQEQ = auto()  # ==
    BANGEQ = auto()  # !=
    LT = auto()  # <
    GT = auto()  # >
    LTEQ = auto()  # <=
    GTEQ = auto()  # >=
    AMPAMP = auto()  # &&
    PIPEPIPE = auto()  # ||

    # Special
    EOF = auto()


KEYWORDS: dict[str, TokenKind] = {
    "fn": TokenKind.FN,
    "let": TokenKind.LET,
    "if": TokenKind.IF,
    "else": TokenKind.ELSE,
    "while": TokenKind.WHILE,
    "return": TokenKind.RETURN,
    "true": TokenKind.TRUE,
    "false": TokenKind.FALSE,
}


@dataclass(frozen=True, slots=True)
class Token:
    kind: TokenKind
    value: str
    line: int
    col: int


class LexError(Exception):
    def __init__(self, message: str, line: int, col: int) -> None:
        super().__init__(f"[{line}:{col}] {message}")
        self.line = line
        self.col = col


def tokenize(source: str) -> list[Token]:
    """Tokenize GoL Script source code into a list of tokens (ending with EOF)."""
    tokens: list[Token] = []
    i = 0
    line = 1
    col = 1

    while i < len(source):
        ch = source[i]

        # Whitespace
        if ch in " \t\r":
            i += 1
            col += 1
            continue

        if ch == "\n":
            i += 1
            line += 1
            col = 1
            continue

        # Comment
        if ch == "/" and i + 1 < len(source) and source[i + 1] == "/":
            while i < len(source) and source[i] != "\n":
                i += 1
            continue

        # Integer literal
        if ch.isdigit():
            start = i
            start_col = col
            while i < len(source) and source[i].isdigit():
                i += 1
                col += 1
            tokens.append(Token(TokenKind.INT_LIT, source[start:i], line, start_col))
            continue

        # Identifier / keyword
        if ch.isalpha() or ch == "_":
            start = i
            start_col = col
            while i < len(source) and (source[i].isalnum() or source[i] == "_"):
                i += 1
                col += 1
            word = source[start:i]
            kind = KEYWORDS.get(word, TokenKind.IDENT)
            tokens.append(Token(kind, word, line, start_col))
            continue

        # Two-character operators
        if i + 1 < len(source):
            two = source[i : i + 2]
            two_char_ops: dict[str, TokenKind] = {
                "==": TokenKind.EQEQ,
                "!=": TokenKind.BANGEQ,
                "<=": TokenKind.LTEQ,
                ">=": TokenKind.GTEQ,
                "&&": TokenKind.AMPAMP,
                "||": TokenKind.PIPEPIPE,
            }
            if two in two_char_ops:
                tokens.append(Token(two_char_ops[two], two, line, col))
                i += 2
                col += 2
                continue

        # Single-character tokens
        single_ops: dict[str, TokenKind] = {
            "(": TokenKind.LPAREN,
            ")": TokenKind.RPAREN,
            "{": TokenKind.LBRACE,
            "}": TokenKind.RBRACE,
            ";": TokenKind.SEMICOLON,
            ",": TokenKind.COMMA,
            "+": TokenKind.PLUS,
            "-": TokenKind.MINUS,
            "*": TokenKind.STAR,
            "/": TokenKind.SLASH,
            "%": TokenKind.PERCENT,
            "!": TokenKind.BANG,
            "=": TokenKind.EQ,
            "<": TokenKind.LT,
            ">": TokenKind.GT,
        }
        if ch in single_ops:
            tokens.append(Token(single_ops[ch], ch, line, col))
            i += 1
            col += 1
            continue

        raise LexError(f"unexpected character '{ch}'", line, col)

    tokens.append(Token(TokenKind.EOF, "", line, col))
    return tokens
