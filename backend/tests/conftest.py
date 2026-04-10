"""Shared test fixtures."""

from __future__ import annotations

import os
from collections.abc import AsyncIterator
from unittest.mock import AsyncMock

import pytest
from httpx import ASGITransport, AsyncClient

# Test environment setup.
os.environ["GOL_RATE_LIMIT"] = "0"
os.environ["GOL_DEBUG"] = "true"

from app.config import Settings
from app.db.engine import create_tables, init_db
from app.main import create_app


@pytest.fixture
def settings() -> Settings:
    return Settings(debug=True, database_url="sqlite+aiosqlite://")


@pytest.fixture
async def client(settings: Settings) -> AsyncIterator[AsyncClient]:
    # Initialize DB before creating the app (the lifespan won't trigger with ASGITransport).
    init_db(settings)
    await create_tables()

    app = create_app(settings)
    # Provide a mock engine so endpoints that access app.state.engine don't crash.
    app.state.engine = AsyncMock()
    transport = ASGITransport(app=app)
    async with AsyncClient(transport=transport, base_url="http://test") as ac:
        yield ac
