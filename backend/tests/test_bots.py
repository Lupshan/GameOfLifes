"""Tests for bot submission API."""

from __future__ import annotations

from httpx import AsyncClient

_PASS = "StrongPass1"


async def _signup_and_get_token(client: AsyncClient, email: str = "bot@test.com") -> str:
    resp = await client.post("/auth/signup", json={"email": email, "password": _PASS})
    return resp.json()["access_token"]


def _auth(token: str) -> dict[str, str]:
    return {"Authorization": f"Bearer {token}"}


async def test_create_bot_valid_source(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client)
    resp = await client.post(
        "/bots",
        json={"name": "walker", "source": "fn main() { move(); }"},
        headers=_auth(token),
    )
    assert resp.status_code == 201
    data = resp.json()
    assert data["name"] == "walker"
    assert data["compile_ok"] is True
    assert data["compile_errors"] is None


async def test_create_bot_invalid_source(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client, "bad@test.com")
    resp = await client.post(
        "/bots",
        json={"name": "broken", "source": "fn main() { @ }"},
        headers=_auth(token),
    )
    assert resp.status_code == 201
    data = resp.json()
    assert data["compile_ok"] is False
    assert data["compile_errors"] is not None
    assert len(data["compile_errors"]) > 0


async def test_list_bots(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client, "list@test.com")
    await client.post(
        "/bots",
        json={"name": "bot1", "source": "fn main() { move(); }"},
        headers=_auth(token),
    )
    await client.post(
        "/bots",
        json={"name": "bot2", "source": "fn main() { eat(); }"},
        headers=_auth(token),
    )

    resp = await client.get("/bots", headers=_auth(token))
    assert resp.status_code == 200
    assert len(resp.json()) == 2


async def test_get_bot_detail(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client, "detail@test.com")
    create_resp = await client.post(
        "/bots",
        json={"name": "detail-bot", "source": "fn main() { move(); }"},
        headers=_auth(token),
    )
    bot_id = create_resp.json()["id"]

    # Now requires auth (B1 fix).
    resp = await client.get(f"/bots/{bot_id}", headers=_auth(token))
    assert resp.status_code == 200
    assert resp.json()["source"] == "fn main() { move(); }"


async def test_get_bot_detail_requires_auth(client: AsyncClient) -> None:
    """B1: GET /bots/{bot_id} must require authentication."""
    resp = await client.get("/bots/nonexistent-id")
    assert resp.status_code == 401


async def test_publish_bot(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client, "pub@test.com")
    create_resp = await client.post(
        "/bots",
        json={"name": "pub-bot", "source": "fn main() { move(); }"},
        headers=_auth(token),
    )
    bot_id = create_resp.json()["id"]

    resp = await client.post(f"/bots/{bot_id}/publish", headers=_auth(token))
    assert resp.status_code == 200
    assert resp.json()["published"] is True


async def test_publish_uncompiled_bot_fails(client: AsyncClient) -> None:
    token = await _signup_and_get_token(client, "fail@test.com")
    create_resp = await client.post(
        "/bots",
        json={"name": "fail-bot", "source": "invalid code @@@"},
        headers=_auth(token),
    )
    bot_id = create_resp.json()["id"]

    resp = await client.post(f"/bots/{bot_id}/publish", headers=_auth(token))
    assert resp.status_code == 400


async def test_create_bot_requires_auth(client: AsyncClient) -> None:
    resp = await client.post("/bots", json={"name": "no-auth", "source": "fn main() {}"})
    assert resp.status_code == 401
