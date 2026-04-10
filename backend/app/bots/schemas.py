"""Pydantic schemas for bot endpoints."""

from __future__ import annotations

import re

from pydantic import BaseModel, Field, field_validator

_BOT_NAME_RE = re.compile(r"^[a-zA-Z0-9][a-zA-Z0-9 _\-]{0,99}$")


class BotCreateRequest(BaseModel):
    name: str = Field(min_length=1, max_length=100)

    @field_validator("name")
    @classmethod
    def validate_name(cls, v: str) -> str:
        if not _BOT_NAME_RE.match(v):
            raise ValueError("Bot name must be alphanumeric (with spaces, hyphens, underscores)")
        return v

    source: str = ""


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
