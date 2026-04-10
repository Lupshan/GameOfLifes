"""FastAPI application factory."""

from __future__ import annotations

import asyncio
import logging
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from pathlib import Path

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from slowapi import _rate_limit_exceeded_handler
from slowapi.errors import RateLimitExceeded
from slowapi.middleware import SlowAPIMiddleware
from starlette.routing import Route

from app.auth.router import router as auth_router
from app.bots.router import router as bots_router
from app.config import Settings, get_settings
from app.db.engine import create_tables, get_session_factory, init_db
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
        from sqlalchemy import select

        from app.bots.service import get_latest_version
        from app.db.models import Bot
        from app.engine.service import EngineService
        from app.ws.router import broadcast

        init_db(settings)  # type: ignore[arg-type]
        await create_tables()

        # Start engine.
        engine = EngineService(settings.engine_path)
        await engine.start()
        _app.state.engine = engine

        # Bridge: engine snapshots → WS broadcast.
        async def _snapshot_bridge() -> None:
            q = engine.subscribe_snapshots()
            try:
                while True:
                    snapshot = await q.get()
                    broadcast(snapshot)
            except asyncio.CancelledError:
                engine.unsubscribe_snapshots(q)

        bridge_task = asyncio.create_task(_snapshot_bridge())

        # Load all published bots into the engine.
        session_factory = get_session_factory()
        async with session_factory() as session:
            result = await session.execute(select(Bot).where(Bot.published.is_(True)))
            for bot in result.scalars().all():
                version = await get_latest_version(bot.id, session)
                if version and version.compile_ok and version.bytecode:
                    await engine.load_bot(bot.id, version.bytecode)

        prune_task = asyncio.create_task(_periodic_prune())

        yield

        bridge_task.cancel()
        prune_task.cancel()
        await engine.stop()

    from app.middleware.rate_limit import limiter

    app = FastAPI(title=settings.app_name, debug=settings.debug, lifespan=lifespan)

    # CORS — restrict to known frontend origins.
    if settings.debug:
        allowed_origins = ["http://localhost:3000", "http://localhost:5173"]
    elif settings.cors_origin:
        allowed_origins = [settings.cors_origin]
    else:
        allowed_origins = []
    app.add_middleware(
        CORSMiddleware,
        allow_origins=allowed_origins,
        allow_credentials=True,
        allow_methods=["GET", "POST", "DELETE"],
        allow_headers=["Content-Type", "Authorization"],
    )

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
