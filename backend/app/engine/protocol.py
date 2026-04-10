"""IPC protocol: commands sent to engine, events received from engine.

All messages are line-delimited JSON over stdin/stdout.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from enum import StrEnum


class CommandType(StrEnum):
    LOAD_BOT = "load_bot"
    UNLOAD_BOT = "unload_bot"
    PAUSE = "pause"
    RESUME = "resume"
    SHUTDOWN = "shutdown"


class EventType(StrEnum):
    SNAPSHOT = "snapshot"
    READY = "ready"
    ERROR = "error"
    ACK = "ack"


@dataclass
class Command:
    cmd: CommandType
    bot_id: str | None = None
    bytecode_b64: str | None = None  # base64-encoded .gob bytes


@dataclass
class Event:
    event: EventType
    tick: int = 0
    data: dict[str, object] = field(default_factory=dict)
    error: str | None = None
