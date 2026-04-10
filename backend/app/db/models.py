"""SQLModel table definitions."""

from __future__ import annotations

import uuid
from datetime import UTC, datetime

from sqlmodel import Field, SQLModel


class User(SQLModel, table=True):
    __tablename__ = "users"

    id: str = Field(default_factory=lambda: uuid.uuid4().hex, primary_key=True)
    email: str = Field(index=True, unique=True)
    password_hash: str
    created_at: datetime = Field(default_factory=lambda: datetime.now(UTC))


class Bot(SQLModel, table=True):
    __tablename__ = "bots"

    id: str = Field(default_factory=lambda: uuid.uuid4().hex, primary_key=True)
    name: str
    user_id: str = Field(foreign_key="users.id", index=True)
    published: bool = Field(default=False)
    created_at: datetime = Field(default_factory=lambda: datetime.now(UTC))
    updated_at: datetime = Field(default_factory=lambda: datetime.now(UTC))


class BotVersion(SQLModel, table=True):
    __tablename__ = "bot_versions"

    id: str = Field(default_factory=lambda: uuid.uuid4().hex, primary_key=True)
    bot_id: str = Field(foreign_key="bots.id", index=True)
    version: int = Field(default=1)
    source: str  # GoL Script source code
    bytecode: bytes | None = Field(default=None)  # Compiled .gob blob
    compile_ok: bool = Field(default=False)
    compile_errors: str | None = Field(default=None)  # JSON string of errors
    created_at: datetime = Field(default_factory=lambda: datetime.now(UTC))


class Snapshot(SQLModel, table=True):
    __tablename__ = "snapshots"

    id: str = Field(default_factory=lambda: uuid.uuid4().hex, primary_key=True)
    tick: int = Field(index=True)
    file_path: str  # Path to compressed snapshot on disk
    size_bytes: int = Field(default=0)
    created_at: datetime = Field(default_factory=lambda: datetime.now(UTC))


class LineageEvent(SQLModel, table=True):
    __tablename__ = "lineage_events"

    id: str = Field(default_factory=lambda: uuid.uuid4().hex, primary_key=True)
    tick: int = Field(index=True)
    event_type: str  # "birth" or "death"
    agent_id: int
    bot_id: str | None = Field(default=None, foreign_key="bots.id", index=True)
    parent_agent_id: int | None = Field(default=None)
    generation: int | None = Field(default=None)
    cause: str | None = Field(default=None)  # For death events
    created_at: datetime = Field(default_factory=lambda: datetime.now(UTC))
