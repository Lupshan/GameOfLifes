"""Tests for the engine bridge (protocol + subprocess IPC)."""

from __future__ import annotations

import json

from app.engine.protocol import Command, CommandType, Event, EventType

# ---- Protocol unit tests ----


def test_command_serialization() -> None:
    cmd = Command(cmd=CommandType.LOAD_BOT, bot_id="bot-1", bytecode_b64="AQID")
    payload = {"cmd": cmd.cmd.value}
    if cmd.bot_id:
        payload["bot_id"] = cmd.bot_id
    if cmd.bytecode_b64:
        payload["bytecode"] = cmd.bytecode_b64
    j = json.dumps(payload)
    parsed = json.loads(j)
    assert parsed["cmd"] == "load_bot"
    assert parsed["bot_id"] == "bot-1"


def test_event_from_json() -> None:
    data = {"event": "snapshot", "tick": 42, "agents": []}
    event = Event(
        event=EventType(data["event"]),
        tick=data.get("tick", 0),
        data=data,
    )
    assert event.event == EventType.SNAPSHOT
    assert event.tick == 42


def test_command_types() -> None:
    assert CommandType.LOAD_BOT.value == "load_bot"
    assert CommandType.SHUTDOWN.value == "shutdown"
    assert CommandType.PAUSE.value == "pause"
    assert CommandType.RESUME.value == "resume"


def test_event_types() -> None:
    assert EventType.SNAPSHOT.value == "snapshot"
    assert EventType.READY.value == "ready"
    assert EventType.ERROR.value == "error"
    assert EventType.ACK.value == "ack"
