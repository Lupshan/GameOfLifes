"""FastAPI application factory."""

from __future__ import annotations

from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from fastapi import FastAPI

from app.auth.router import router as auth_router
from app.bots.router import router as bots_router
from app.config import Settings, get_settings
from app.db.engine import create_tables, init_db
from app.health.router import router as health_router
from app.ws.router import router as ws_router


def create_app(settings: Settings | None = None) -> FastAPI:
    if settings is None:
        settings = get_settings()

    @asynccontextmanager
    async def lifespan(_app: FastAPI) -> AsyncIterator[None]:
        init_db(settings)  # type: ignore[arg-type]
        await create_tables()
        yield

    app = FastAPI(title=settings.app_name, debug=settings.debug, lifespan=lifespan)
    app.include_router(health_router)
    app.include_router(auth_router)
    app.include_router(bots_router)
    app.include_router(ws_router)
    return app


app = create_app()
