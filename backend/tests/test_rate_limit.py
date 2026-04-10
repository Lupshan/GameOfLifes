"""Tests for rate limiting middleware."""

from __future__ import annotations

from collections.abc import AsyncIterator

import pytest
from httpx import ASGITransport, AsyncClient

from app.config import Settings
from app.db.engine import create_tables, init_db
from app.main import create_app
from app.middleware.rate_limit import limiter


@pytest.fixture
async def limited_client() -> AsyncIterator[AsyncClient]:
    settings = Settings(debug=True, database_url="sqlite+aiosqlite://")
    init_db(settings)
    await create_tables()

    # Force-enable the limiter for this test (conftest disables it globally).
    limiter.enabled = True

    app = create_app(settings)
    transport = ASGITransport(app=app)
    async with AsyncClient(transport=transport, base_url="http://test") as ac:
        yield ac

    limiter.enabled = False


async def test_rate_limit_returns_429(limited_client: AsyncClient) -> None:
    """Exceeding the global rate limit should return 429."""
    # The default limit is 60/minute. Send 65 requests quickly.
    statuses = []
    for _ in range(65):
        resp = await limited_client.get("/health")
        statuses.append(resp.status_code)

    assert 429 in statuses, "Expected at least one 429 response after exceeding rate limit"
