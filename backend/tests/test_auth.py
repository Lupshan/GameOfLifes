"""Tests for auth endpoints."""

from __future__ import annotations

from httpx import AsyncClient

from app.auth.service import create_token, decode_token, hash_password, verify_password
from app.config import Settings

# ---- service unit tests ----


def test_hash_and_verify_password() -> None:
    hashed = hash_password("secret123")
    assert verify_password("secret123", hashed)
    assert not verify_password("wrong", hashed)


def test_create_and_decode_token() -> None:
    settings = Settings(jwt_secret="test-secret")
    token = create_token("user-42", settings)
    user_id = decode_token(token, settings)
    assert user_id == "user-42"


def test_decode_invalid_token() -> None:
    settings = Settings(jwt_secret="test-secret")
    assert decode_token("invalid.token.here", settings) is None


def test_decode_wrong_secret() -> None:
    settings1 = Settings(jwt_secret="secret-1")
    settings2 = Settings(jwt_secret="secret-2")
    token = create_token("user-1", settings1)
    assert decode_token(token, settings2) is None


# ---- endpoint integration tests ----


async def test_signup_happy_path(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "new@test.com", "password": "pass123"})
    assert resp.status_code == 201
    data = resp.json()
    assert "access_token" in data
    assert data["token_type"] == "bearer"


async def test_signup_duplicate_email(client: AsyncClient) -> None:
    await client.post("/auth/signup", json={"email": "dup@test.com", "password": "pass123"})
    resp = await client.post("/auth/signup", json={"email": "dup@test.com", "password": "pass456"})
    assert resp.status_code == 409


async def test_login_happy_path(client: AsyncClient) -> None:
    await client.post("/auth/signup", json={"email": "login@test.com", "password": "pass123"})
    resp = await client.post("/auth/login", json={"email": "login@test.com", "password": "pass123"})
    assert resp.status_code == 200
    assert "access_token" in resp.json()


async def test_login_wrong_password(client: AsyncClient) -> None:
    await client.post("/auth/signup", json={"email": "wrong@test.com", "password": "pass123"})
    resp = await client.post("/auth/login", json={"email": "wrong@test.com", "password": "bad"})
    assert resp.status_code == 401


async def test_login_unknown_email(client: AsyncClient) -> None:
    resp = await client.post("/auth/login", json={"email": "no@test.com", "password": "pass"})
    assert resp.status_code == 401


async def test_me_with_valid_token(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "me@test.com", "password": "pass123"})
    token = resp.json()["access_token"]

    resp = await client.get("/auth/me", headers={"Authorization": f"Bearer {token}"})
    assert resp.status_code == 200
    assert resp.json()["email"] == "me@test.com"


async def test_me_without_token(client: AsyncClient) -> None:
    resp = await client.get("/auth/me")
    assert resp.status_code == 403  # HTTPBearer returns 403 if no credentials


async def test_me_with_invalid_token(client: AsyncClient) -> None:
    resp = await client.get("/auth/me", headers={"Authorization": "Bearer invalid"})
    assert resp.status_code == 401
