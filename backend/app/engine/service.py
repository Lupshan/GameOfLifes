"""EngineService — high-level API for controlling the simulation engine."""

from __future__ import annotations

import asyncio
import base64
import contextlib
import logging

from app.engine.process import EngineProcess
from app.engine.protocol import Command, CommandType, EventType

logger = logging.getLogger(__name__)


class EngineService:
    """High-level interface to the engine subprocess."""

    def __init__(self, engine_path: str | None = None) -> None:
        self._process = EngineProcess(engine_path) if engine_path else EngineProcess()
        self._snapshot_subscribers: list[asyncio.Queue[dict[str, object]]] = []
        self._dispatcher_task: asyncio.Task[None] | None = None

    async def start(self) -> None:
        await self._process.start()
        self._dispatcher_task = asyncio.create_task(self._dispatch_events())

    async def stop(self) -> None:
        await self._process.stop()
        if self._dispatcher_task:
            self._dispatcher_task.cancel()

    async def load_bot(self, bot_id: str, bytecode: bytes) -> None:
        """Load a compiled bot into the engine."""
        b64 = base64.b64encode(bytecode).decode("ascii")
        await self._process.send_command(
            Command(cmd=CommandType.LOAD_BOT, bot_id=bot_id, bytecode_b64=b64)
        )

    async def unload_bot(self, bot_id: str) -> None:
        await self._process.send_command(Command(cmd=CommandType.UNLOAD_BOT, bot_id=bot_id))

    async def pause(self) -> None:
        await self._process.send_command(Command(cmd=CommandType.PAUSE))

    async def resume(self) -> None:
        await self._process.send_command(Command(cmd=CommandType.RESUME))

    def subscribe_snapshots(self) -> asyncio.Queue[dict[str, object]]:
        """Subscribe to snapshot events. Returns a queue that receives snapshot dicts."""
        q: asyncio.Queue[dict[str, object]] = asyncio.Queue(maxsize=100)
        self._snapshot_subscribers.append(q)
        return q

    def unsubscribe_snapshots(self, q: asyncio.Queue[dict[str, object]]) -> None:
        self._snapshot_subscribers = [s for s in self._snapshot_subscribers if s is not q]

    async def _dispatch_events(self) -> None:
        """Background: read events from process and dispatch to subscribers."""
        try:
            while True:
                event = await self._process.recv_event(timeout=1.0)
                if event is None:
                    if not self._process.running:
                        logger.warning("Engine process exited, attempting restart...")
                        await self._process.start()
                    continue

                if event.event == EventType.SNAPSHOT:
                    for q in list(self._snapshot_subscribers):
                        if q.full():
                            with contextlib.suppress(asyncio.QueueEmpty):
                                q.get_nowait()
                        await q.put(event.data)

                elif event.event == EventType.ERROR:
                    logger.error("Engine error: %s", event.error)

        except asyncio.CancelledError:
            pass
