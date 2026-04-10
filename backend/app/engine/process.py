"""Engine subprocess manager — spawns and communicates with the C++ engine."""

from __future__ import annotations

import asyncio
import contextlib
import json
import logging
from pathlib import Path

from app.engine.protocol import Command, CommandType, Event, EventType

logger = logging.getLogger(__name__)

DEFAULT_ENGINE_PATH = "engine/build/gameoflifes_engine"


class EngineProcess:
    """Manages a single engine subprocess with JSON-over-stdio IPC."""

    def __init__(self, engine_path: str | Path = DEFAULT_ENGINE_PATH) -> None:
        self._engine_path = str(engine_path)
        self._process: asyncio.subprocess.Process | None = None
        self._event_queue: asyncio.Queue[Event] = asyncio.Queue(maxsize=1000)
        self._reader_task: asyncio.Task[None] | None = None
        self._running = False

    @property
    def running(self) -> bool:
        return self._running and self._process is not None and self._process.returncode is None

    async def start(self) -> None:
        """Start the engine subprocess in --ipc mode."""
        self._process = await asyncio.create_subprocess_exec(
            self._engine_path,
            "--ipc",
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )
        self._running = True
        self._reader_task = asyncio.create_task(self._read_events())
        logger.info("Engine process started (pid=%s)", self._process.pid)

    async def stop(self) -> None:
        """Send shutdown command and wait for the process to exit."""
        if self._process is None:
            return
        try:
            await self.send_command(Command(cmd=CommandType.SHUTDOWN))
            await asyncio.wait_for(self._process.wait(), timeout=5.0)
        except (TimeoutError, ProcessLookupError):
            self._process.kill()
            await self._process.wait()
        self._running = False
        if self._reader_task:
            self._reader_task.cancel()
        logger.info("Engine process stopped")

    async def send_command(self, command: Command) -> None:
        """Send a command to the engine via stdin."""
        if self._process is None or self._process.stdin is None:
            raise RuntimeError("Engine process not started")

        payload: dict[str, object] = {"cmd": command.cmd.value}
        if command.bot_id is not None:
            payload["bot_id"] = command.bot_id
        if command.bytecode_b64 is not None:
            payload["bytecode"] = command.bytecode_b64

        line = json.dumps(payload) + "\n"
        self._process.stdin.write(line.encode())
        await self._process.stdin.drain()

    async def recv_event(self, timeout: float = 5.0) -> Event | None:
        """Receive the next event from the engine."""
        try:
            return await asyncio.wait_for(self._event_queue.get(), timeout=timeout)
        except TimeoutError:
            return None

    async def _read_events(self) -> None:
        """Background task: read events from engine stdout."""
        if self._process is None or self._process.stdout is None:
            return
        try:
            while True:
                line = await self._process.stdout.readline()
                if not line:
                    break
                try:
                    data = json.loads(line)
                    raw_event = data.get("event")
                    if raw_event is None:
                        logger.warning(
                            "Engine event missing 'event' field: %s", line.decode().rstrip()
                        )
                        continue
                    event = Event(
                        event=EventType(raw_event),
                        tick=data.get("tick", 0),
                        data=data,
                        error=data.get("error"),
                    )
                    if self._event_queue.full():
                        # Drop oldest event if queue is full (backpressure).
                        with contextlib.suppress(asyncio.QueueEmpty):
                            self._event_queue.get_nowait()
                    await self._event_queue.put(event)
                except (json.JSONDecodeError, ValueError) as e:
                    logger.warning(
                        "Failed to parse engine event: %s (raw: %s)", e, line.decode().rstrip()
                    )
        except asyncio.CancelledError:
            pass
        except OSError:
            logger.exception("Engine reader task crashed")
        finally:
            self._running = False
