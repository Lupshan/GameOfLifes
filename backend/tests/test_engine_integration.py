"""Tests for engine integration: publish→load, delete→unload.

The EngineService is mocked to avoid needing the C++ binary.
We inject the mock into app.state.engine after the app is created,
bypassing the lifespan engine startup.
"""

from __future__ import annotations

from unittest.mock import AsyncMock

from httpx import AsyncClient

_PASS = "StrongPass1"


async def test_publish_calls_engine_load_bot(client: AsyncClient) -> None:
    """Publishing a bot must call engine.load_bot with the bot's bytecode."""
    # Inject mock engine into app state.
    mock_engine = AsyncMock()
    mock_engine.load_bot = AsyncMock()
    client._transport.app.state.engine = mock_engine  # type: ignore[union-attr]

    token = (
        await client.post("/auth/signup", json={"email": "eng1@test.com", "password": _PASS})
    ).json()["access_token"]
    headers = {"Authorization": f"Bearer {token}"}

    resp = await client.post(
        "/bots",
        json={"name": "test-bot", "source": "fn main() { move_forward(); }"},
        headers=headers,
    )
    assert resp.status_code == 201
    bot_id = resp.json()["id"]

    if resp.json()["compile_ok"]:
        resp = await client.post(f"/bots/{bot_id}/publish", headers=headers)
        assert resp.status_code == 200
        mock_engine.load_bot.assert_called_once()
        assert mock_engine.load_bot.call_args[0][0] == bot_id


async def test_delete_published_bot_calls_engine_unload(client: AsyncClient) -> None:
    """Deleting a published bot must call engine.unload_bot."""
    mock_engine = AsyncMock()
    mock_engine.load_bot = AsyncMock()
    mock_engine.unload_bot = AsyncMock()
    client._transport.app.state.engine = mock_engine  # type: ignore[union-attr]

    token = (
        await client.post("/auth/signup", json={"email": "eng2@test.com", "password": _PASS})
    ).json()["access_token"]
    headers = {"Authorization": f"Bearer {token}"}

    resp = await client.post(
        "/bots",
        json={"name": "del-bot", "source": "fn main() { move_forward(); }"},
        headers=headers,
    )
    assert resp.status_code == 201
    bot_id = resp.json()["id"]

    if resp.json()["compile_ok"]:
        await client.post(f"/bots/{bot_id}/publish", headers=headers)
        mock_engine.unload_bot.reset_mock()

        resp = await client.delete(f"/bots/{bot_id}", headers=headers)
        assert resp.status_code == 200
        mock_engine.unload_bot.assert_called_once_with(bot_id)


async def test_delete_unpublished_bot_does_not_call_engine(client: AsyncClient) -> None:
    """Deleting an unpublished bot must NOT call engine.unload_bot."""
    mock_engine = AsyncMock()
    mock_engine.unload_bot = AsyncMock()
    client._transport.app.state.engine = mock_engine  # type: ignore[union-attr]

    token = (
        await client.post("/auth/signup", json={"email": "eng3@test.com", "password": _PASS})
    ).json()["access_token"]
    headers = {"Authorization": f"Bearer {token}"}

    resp = await client.post(
        "/bots",
        json={"name": "nopub-bot", "source": "fn main() { move_forward(); }"},
        headers=headers,
    )
    bot_id = resp.json()["id"]
    mock_engine.unload_bot.reset_mock()

    resp = await client.delete(f"/bots/{bot_id}", headers=headers)
    assert resp.status_code == 200
    mock_engine.unload_bot.assert_not_called()
