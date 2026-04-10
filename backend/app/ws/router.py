"""WebSocket endpoint for live world streaming."""

from __future__ import annotations

import asyncio
import contextlib
import json
import logging

from fastapi import APIRouter, WebSocket, WebSocketDisconnect

router = APIRouter()

logger = logging.getLogger(__name__)

# In-memory subscriber list. In production, this would be managed by EngineService.
# For now, we provide a simple pub/sub that can be wired to EngineService later.
_subscribers: list[asyncio.Queue[dict[str, object]]] = []


def get_subscribers() -> list[asyncio.Queue[dict[str, object]]]:
    return _subscribers


def broadcast(snapshot: dict[str, object]) -> None:
    """Push a snapshot to all subscribers. Non-blocking, drops if full."""
    for q in list(_subscribers):
        if q.full():
            with contextlib.suppress(asyncio.QueueEmpty):
                q.get_nowait()
        with contextlib.suppress(asyncio.QueueFull):
            q.put_nowait(snapshot)


@router.websocket("/ws/world")
async def world_stream(ws: WebSocket) -> None:
    await ws.accept()

    q: asyncio.Queue[dict[str, object]] = asyncio.Queue(maxsize=100)
    _subscribers.append(q)
    logger.info("WS client connected (%d total)", len(_subscribers))

    try:
        while True:
            snapshot = await q.get()
            await ws.send_text(json.dumps(snapshot))
    except WebSocketDisconnect:
        pass
    except Exception:
        logger.exception("WS client error")
    finally:
        _subscribers.remove(q)
        logger.info("WS client disconnected (%d remaining)", len(_subscribers))
