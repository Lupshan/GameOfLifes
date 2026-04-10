"""Health check endpoint with DB and engine status."""

from __future__ import annotations

from fastapi import APIRouter, Depends
from sqlalchemy import text
from sqlalchemy.exc import SQLAlchemyError
from sqlalchemy.ext.asyncio import AsyncSession

from app.db.engine import get_session

router = APIRouter()


@router.get("/health")
async def health(session: AsyncSession = Depends(get_session)) -> dict[str, object]:
    checks: dict[str, object] = {"status": "ok"}

    # DB check.
    try:
        await session.execute(text("SELECT 1"))
        checks["db"] = "ok"
    except (SQLAlchemyError, OSError):
        checks["db"] = "error"
        checks["status"] = "degraded"

    return checks
