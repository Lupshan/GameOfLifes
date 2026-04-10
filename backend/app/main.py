"""FastAPI application factory."""

from __future__ import annotations

import asyncio
import logging
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from pathlib import Path

from fastapi import FastAPI
from slowapi import _rate_limit_exceeded_handler
from slowapi.errors import RateLimitExceeded
from slowapi.middleware import SlowAPIMiddleware
from starlette.routing import Route

from app.auth.router import router as auth_router
from app.bots.router import router as bots_router
from app.config import Settings, get_settings
from app.db.engine import create_tables, init_db
from app.health.router import router as health_router
from app.logging_config import setup_logging
from app.metrics import metrics_endpoint
from app.sims.router import router as sims_router
from app.snapshots.router import lineage_router
from app.snapshots.router import router as snapshots_router
from app.stats.router import router as stats_router
from app.tournaments.router import router as tournaments_router
from app.ws.router import router as ws_router


def create_app(settings: Settings | None = None) -> FastAPI:
    if settings is None:
        settings = get_settings()

    setup_logging(settings.debug)

    _prune_logger = logging.getLogger("app.prune")

    async def _periodic_prune(interval: int = 3600, keep_days: int = 7) -> None:
        """Prune old snapshots every `interval` seconds."""
        from app.snapshots.service import prune

        snap_dir = Path("snapshots")
        while True:
            await asyncio.sleep(interval)
            try:
                deleted = await prune(snap_dir, keep_days)
                if deleted > 0:
                    _prune_logger.info("Pruned %d snapshots", deleted)
            except Exception:
                _prune_logger.exception("Snapshot pruning failed")

    @asynccontextmanager
    async def lifespan(_app: FastAPI) -> AsyncIterator[None]:
        init_db(settings)  # type: ignore[arg-type]
        await create_tables()
        prune_task = asyncio.create_task(_periodic_prune())
        yield
        prune_task.cancel()

    from app.middleware.rate_limit import limiter

    app = FastAPI(title=settings.app_name, debug=settings.debug, lifespan=lifespan)
    app.state.limiter = limiter
    app.add_exception_handler(RateLimitExceeded, _rate_limit_exceeded_handler)
    app.add_middleware(SlowAPIMiddleware)
    app.routes.append(Route("/metrics", metrics_endpoint))
    app.include_router(health_router)
    app.include_router(auth_router)
    app.include_router(bots_router)
    app.include_router(ws_router)
    app.include_router(snapshots_router)
    app.include_router(lineage_router)
    app.include_router(stats_router)
    app.include_router(sims_router)
    app.include_router(tournaments_router)
    return app


app = create_app()
