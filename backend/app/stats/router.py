"""Stats endpoint: aggregated simulation statistics.

Aggregation happens in SQL (GROUP BY) to avoid loading all events into memory.
"""

from __future__ import annotations

from fastapi import APIRouter, Depends, Query
from pydantic import BaseModel
from sqlalchemy import case, func, literal_column, select
from sqlalchemy.ext.asyncio import AsyncSession

from app.db.engine import get_session
from app.db.models import LineageEvent

router = APIRouter(prefix="/stats", tags=["stats"])


class TickBucket(BaseModel):
    tick_bucket: int
    births: int
    deaths: int


class StatsResponse(BaseModel):
    buckets: list[TickBucket]
    total_births: int
    total_deaths: int


@router.get("", response_model=StatsResponse)
async def get_stats(
    bot_id: str | None = None,
    from_tick: int = Query(0, alias="from"),
    to_tick: int = Query(999999999, alias="to"),
    bucket_size: int = Query(100),
    session: AsyncSession = Depends(get_session),
) -> StatsResponse:
    bucket_expr = (LineageEvent.tick / bucket_size * bucket_size).label("bucket")
    births_expr = func.sum(case((LineageEvent.event_type == "birth", 1), else_=0)).label("births")
    deaths_expr = func.sum(case((LineageEvent.event_type == "death", 1), else_=0)).label("deaths")

    stmt = (
        select(bucket_expr, births_expr, deaths_expr)
        .where(LineageEvent.tick >= from_tick, LineageEvent.tick <= to_tick)
        .group_by(literal_column("bucket"))
        .order_by(literal_column("bucket"))
    )
    if bot_id is not None:
        stmt = stmt.where(LineageEvent.bot_id == bot_id)

    result = await session.execute(stmt)
    rows = result.all()

    total_births = 0
    total_deaths = 0
    buckets: list[TickBucket] = []
    for row in rows:
        b = int(row[0])
        births = int(row[1])
        deaths = int(row[2])
        total_births += births
        total_deaths += deaths
        buckets.append(TickBucket(tick_bucket=b, births=births, deaths=deaths))

    return StatsResponse(buckets=buckets, total_births=total_births, total_deaths=total_deaths)
