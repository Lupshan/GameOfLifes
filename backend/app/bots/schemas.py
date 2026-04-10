"""Pydantic schemas for bot endpoints."""

from __future__ import annotations

from pydantic import BaseModel


class BotCreateRequest(BaseModel):
    name: str
    source: str = ""
    kind: str = "bytecode"  # "bytecode" or "ml"
    model: str | None = None  # JSON model definition for kind=ml


class CompileErrorResponse(BaseModel):
    message: str
    line: int
    col: int


class BotResponse(BaseModel):
    id: str
    name: str
    published: bool
    compile_ok: bool
    compile_errors: list[CompileErrorResponse] | None = None


class BotDetailResponse(BotResponse):
    source: str
