"""Functional tests for the full auth flow.

These tests exercise the complete signup → cookie → /me → logout → denied
cycle, simulating how a real browser interacts with the API via httpOnly
cookies (not Bearer headers).
"""

from __future__ import annotations

from httpx import AsyncClient

_PASS = "StrongPass1"


# ---- full flow: signup → cookie → me → logout → denied ----


async def test_signup_sets_cookie(client: AsyncClient) -> None:
    """Signup response must include a Set-Cookie header with gol_token."""
    resp = await client.post("/auth/signup", json={"email": "cookie@test.com", "password": _PASS})
    assert resp.status_code == 201
    assert "gol_token" in resp.cookies


async def test_signup_cookie_grants_access_to_me(client: AsyncClient) -> None:
    """After signup, the cookie alone (no Bearer header) must authenticate /me."""
    resp = await client.post("/auth/signup", json={"email": "flow@test.com", "password": _PASS})
    assert resp.status_code == 201
    token_cookie = resp.cookies["gol_token"]

    me_resp = await client.get("/auth/me", cookies={"gol_token": token_cookie})
    assert me_resp.status_code == 200
    assert me_resp.json()["email"] == "flow@test.com"


async def test_login_sets_cookie(client: AsyncClient) -> None:
    """Login response must include a Set-Cookie header with gol_token."""
    await client.post("/auth/signup", json={"email": "logcook@test.com", "password": _PASS})
    resp = await client.post("/auth/login", json={"email": "logcook@test.com", "password": _PASS})
    assert resp.status_code == 200
    assert "gol_token" in resp.cookies


async def test_full_auth_lifecycle(client: AsyncClient) -> None:
    """Signup → cookie auth → logout → cookie rejected."""
    # 1. Signup
    signup_resp = await client.post(
        "/auth/signup", json={"email": "lifecycle@test.com", "password": _PASS}
    )
    assert signup_resp.status_code == 201
    token_cookie = signup_resp.cookies["gol_token"]

    # 2. Authenticated request via cookie
    me_resp = await client.get("/auth/me", cookies={"gol_token": token_cookie})
    assert me_resp.status_code == 200
    assert me_resp.json()["email"] == "lifecycle@test.com"

    # 3. Logout clears the cookie
    logout_resp = await client.post("/auth/logout", cookies={"gol_token": token_cookie})
    assert logout_resp.status_code == 200
    # The response should instruct the browser to delete the cookie.
    set_cookie_header = logout_resp.headers.get("set-cookie", "")
    assert "gol_token" in set_cookie_header

    # 4. /me without cookie must fail
    me_resp2 = await client.get("/auth/me")
    assert me_resp2.status_code == 401


async def test_login_cookie_auth_flow(client: AsyncClient) -> None:
    """Signup → login → use login cookie for /me."""
    await client.post("/auth/signup", json={"email": "loginflow@test.com", "password": _PASS})
    login_resp = await client.post(
        "/auth/login", json={"email": "loginflow@test.com", "password": _PASS}
    )
    assert login_resp.status_code == 200
    token_cookie = login_resp.cookies["gol_token"]

    me_resp = await client.get("/auth/me", cookies={"gol_token": token_cookie})
    assert me_resp.status_code == 200
    assert me_resp.json()["email"] == "loginflow@test.com"


# ---- input validation ----


async def test_signup_invalid_email(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "not-an-email", "password": _PASS})
    assert resp.status_code == 422


async def test_signup_password_too_short(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "short@test.com", "password": "Ab1"})
    assert resp.status_code == 422


async def test_signup_password_no_digit(client: AsyncClient) -> None:
    resp = await client.post(
        "/auth/signup", json={"email": "nodig@test.com", "password": "Abcdefghij"}
    )
    assert resp.status_code == 422


async def test_signup_password_no_uppercase(client: AsyncClient) -> None:
    resp = await client.post(
        "/auth/signup", json={"email": "noup@test.com", "password": "abcdefghi1"}
    )
    assert resp.status_code == 422


async def test_signup_missing_fields(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={})
    assert resp.status_code == 422


async def test_signup_empty_body(client: AsyncClient) -> None:
    resp = await client.post(
        "/auth/signup", content="", headers={"Content-Type": "application/json"}
    )
    assert resp.status_code == 422


# ---- email normalisation ----


async def test_signup_email_normalised_to_lowercase(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "MiXeD@Test.COM", "password": _PASS})
    assert resp.status_code == 201
    token = resp.cookies["gol_token"]

    me_resp = await client.get("/auth/me", cookies={"gol_token": token})
    assert me_resp.json()["email"] == "mixed@test.com"


# ---- response shape ----


async def test_signup_response_has_token_fields(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "shape@test.com", "password": _PASS})
    data = resp.json()
    assert "access_token" in data
    assert data["token_type"] == "bearer"


async def test_me_response_shape(client: AsyncClient) -> None:
    resp = await client.post("/auth/signup", json={"email": "mshape@test.com", "password": _PASS})
    token = resp.cookies["gol_token"]

    me_resp = await client.get("/auth/me", cookies={"gol_token": token})
    data = me_resp.json()
    assert "id" in data
    assert "email" in data
    # Must NOT leak password hash.
    assert "password_hash" not in data
    assert "password" not in data
