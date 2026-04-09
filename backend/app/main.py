"""FastAPI application factory."""

from __future__ import annotations

from fastapi import FastAPI

from app.config import Settings, get_settings
from app.health.router import router as health_router


def create_app(settings: Settings | None = None) -> FastAPI:
    if settings is None:
        settings = get_settings()

    app = FastAPI(title=settings.app_name, debug=settings.debug)
    app.include_router(health_router)
    return app


app = create_app()
