"""Tests for rate limiting middleware."""

from __future__ import annotations

import os
from collections.abc import AsyncIterator

import pytest
from httpx import ASGITransport, AsyncClient

# Enable rate limiting for this test module.
os.environ["GOL_RATE_LIMIT"] = "1"

from app.config import Settings
from app.db.engine import create_tables, init_db
from app.main import create_app


@pytest.fixture
async def limited_client() -> AsyncIterator[AsyncClient]:
    settings = Settings(debug=True, database_url="sqlite+aiosqlite://")
    init_db(settings)
    await create_tables()

    app = create_app(settings)
    transport = ASGITransport(app=app)
    async with AsyncClient(transport=transport, base_url="http://test") as ac:
        yield ac


async def test_rate_limit_returns_429(limited_client: AsyncClient) -> None:
    """Exceeding the global rate limit should return 429."""
    # The default limit is 60/minute. Send 65 requests quickly.
    statuses = []
    for _ in range(65):
        resp = await limited_client.get("/health")
        statuses.append(resp.status_code)

    assert 429 in statuses, "Expected at least one 429 response after exceeding rate limit"
