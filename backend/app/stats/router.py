"""Stats endpoint: aggregated simulation statistics."""

from __future__ import annotations

from fastapi import APIRouter, Depends, Query
from pydantic import BaseModel
from sqlalchemy import select
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
    stmt = select(LineageEvent).where(LineageEvent.tick >= from_tick, LineageEvent.tick <= to_tick)
    if bot_id is not None:
        stmt = stmt.where(LineageEvent.bot_id == bot_id)

    result = await session.execute(stmt)
    events = result.scalars().all()

    # Aggregate into buckets.
    birth_counts: dict[int, int] = {}
    death_counts: dict[int, int] = {}
    total_births = 0
    total_deaths = 0

    for e in events:
        bucket = (e.tick // bucket_size) * bucket_size
        if e.event_type == "birth":
            birth_counts[bucket] = birth_counts.get(bucket, 0) + 1
            total_births += 1
        elif e.event_type == "death":
            death_counts[bucket] = death_counts.get(bucket, 0) + 1
            total_deaths += 1

    all_buckets = sorted(set(birth_counts.keys()) | set(death_counts.keys()))
    buckets = [
        TickBucket(
            tick_bucket=b,
            births=birth_counts.get(b, 0),
            deaths=death_counts.get(b, 0),
        )
        for b in all_buckets
    ]

    return StatsResponse(buckets=buckets, total_births=total_births, total_deaths=total_deaths)
