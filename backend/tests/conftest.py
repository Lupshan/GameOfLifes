"""Shared test fixtures."""

from __future__ import annotations

from collections.abc import AsyncIterator

import pytest
from httpx import ASGITransport, AsyncClient

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
    transport = ASGITransport(app=app)
    async with AsyncClient(transport=transport, base_url="http://test") as ac:
        yield ac
