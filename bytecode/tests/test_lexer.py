"""Tests for the GoL Script lexer."""

import pytest

from compiler.lexer import LexError, Token, TokenKind, tokenize


def test_empty_source() -> None:
    tokens = tokenize("")
    assert len(tokens) == 1
    assert tokens[0].kind == TokenKind.EOF


def test_integer_literal() -> None:
    tokens = tokenize("42")
    assert tokens[0] == Token(TokenKind.INT_LIT, "42", 1, 1)


def test_keywords() -> None:
    source = "fn let if else while return true false"
    tokens = tokenize(source)
    kinds = [t.kind for t in tokens if t.kind != TokenKind.EOF]
    assert kinds == [
        TokenKind.FN,
        TokenKind.LET,
        TokenKind.IF,
        TokenKind.ELSE,
        TokenKind.WHILE,
        TokenKind.RETURN,
        TokenKind.TRUE,
        TokenKind.FALSE,
    ]


def test_identifiers() -> None:
    tokens = tokenize("foo bar_42 _x")
    kinds = [t.kind for t in tokens if t.kind != TokenKind.EOF]
    assert all(k == TokenKind.IDENT for k in kinds)
    values = [t.value for t in tokens if t.kind == TokenKind.IDENT]
    assert values == ["foo", "bar_42", "_x"]


def test_operators() -> None:
    source = "+ - * / % ! = == != < > <= >= && ||"
    tokens = tokenize(source)
    kinds = [t.kind for t in tokens if t.kind != TokenKind.EOF]
    assert kinds == [
        TokenKind.PLUS,
        TokenKind.MINUS,
        TokenKind.STAR,
        TokenKind.SLASH,
        TokenKind.PERCENT,
        TokenKind.BANG,
        TokenKind.EQ,
        TokenKind.EQEQ,
        TokenKind.BANGEQ,
        TokenKind.LT,
        TokenKind.GT,
        TokenKind.LTEQ,
        TokenKind.GTEQ,
        TokenKind.AMPAMP,
        TokenKind.PIPEPIPE,
    ]


def test_punctuation() -> None:
    tokens = tokenize("(){};,")
    kinds = [t.kind for t in tokens if t.kind != TokenKind.EOF]
    assert kinds == [
        TokenKind.LPAREN,
        TokenKind.RPAREN,
        TokenKind.LBRACE,
        TokenKind.RBRACE,
        TokenKind.SEMICOLON,
        TokenKind.COMMA,
    ]


def test_comment_ignored() -> None:
    tokens = tokenize("42 // this is a comment\n7")
    values = [t.value for t in tokens if t.kind == TokenKind.INT_LIT]
    assert values == ["42", "7"]


def test_line_tracking() -> None:
    tokens = tokenize("a\nb\nc")
    lines = [(t.value, t.line) for t in tokens if t.kind == TokenKind.IDENT]
    assert lines == [("a", 1), ("b", 2), ("c", 3)]


def test_unexpected_character() -> None:
    with pytest.raises(LexError):
        tokenize("@")


def test_full_function() -> None:
    source = """fn main() {
        let x = 42;
        move();
    }"""
    tokens = tokenize(source)
    kinds = [t.kind for t in tokens if t.kind != TokenKind.EOF]
    assert TokenKind.FN in kinds
    assert TokenKind.LET in kinds
    assert TokenKind.INT_LIT in kinds
