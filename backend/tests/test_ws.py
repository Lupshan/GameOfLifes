"""Tests for the WebSocket world stream."""

from __future__ import annotations

from app.config import Settings
from app.db.engine import create_tables, init_db
from app.main import create_app
from app.ws.router import broadcast, get_subscribers


async def test_ws_connect_and_receive() -> None:
    settings = Settings(debug=True, database_url="sqlite+aiosqlite://")
    init_db(settings)
    await create_tables()
    app = create_app(settings)

    from starlette.testclient import TestClient

    # Use Starlette's sync TestClient for WebSocket testing.
    client = TestClient(app)

    with client.websocket_connect("/ws/world") as ws:
        # Broadcast a snapshot.
        snapshot = {"event": "snapshot", "tick": 1, "agents": []}
        broadcast(snapshot)

        data = ws.receive_json()
        assert data["tick"] == 1
        assert data["event"] == "snapshot"


async def test_ws_disconnect_cleanup() -> None:
    settings = Settings(debug=True, database_url="sqlite+aiosqlite://")
    init_db(settings)
    await create_tables()
    app = create_app(settings)

    from starlette.testclient import TestClient

    client = TestClient(app)

    subs_before = len(get_subscribers())
    with client.websocket_connect("/ws/world"):
        assert len(get_subscribers()) == subs_before + 1
    # After disconnect, subscriber should be removed.
    assert len(get_subscribers()) == subs_before


async def test_ws_multi_client() -> None:
    settings = Settings(debug=True, database_url="sqlite+aiosqlite://")
    init_db(settings)
    await create_tables()
    app = create_app(settings)

    from starlette.testclient import TestClient

    client = TestClient(app)

    with (
        client.websocket_connect("/ws/world") as ws1,
        client.websocket_connect("/ws/world") as ws2,
    ):
        snapshot = {"event": "snapshot", "tick": 42}
        broadcast(snapshot)

        d1 = ws1.receive_json()
        d2 = ws2.receive_json()
        assert d1["tick"] == 42
        assert d2["tick"] == 42


async def test_broadcast_no_subscribers() -> None:
    # Should not raise even with no subscribers.
    broadcast({"event": "snapshot", "tick": 0})
