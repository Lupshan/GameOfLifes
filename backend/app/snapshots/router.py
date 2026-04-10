"""Snapshot and lineage REST endpoints."""

from __future__ import annotations

from pathlib import Path

from fastapi import APIRouter, Depends, HTTPException, Query, status
from pydantic import BaseModel
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.db.engine import get_session
from app.db.models import LineageEvent, Snapshot
from app.snapshots.storage import read_snapshot

router = APIRouter(prefix="/snapshots", tags=["snapshots"])


class SnapshotMeta(BaseModel):
    id: str
    tick: int
    file_path: str
    size_bytes: int


class LineageEventResponse(BaseModel):
    id: str
    tick: int
    event_type: str
    agent_id: int
    bot_id: str | None
    parent_agent_id: int | None
    generation: int | None
    cause: str | None


@router.get("", response_model=list[SnapshotMeta])
async def list_snapshots(
    from_tick: int = Query(0, alias="from"),
    to_tick: int = Query(999999999, alias="to"),
    session: AsyncSession = Depends(get_session),
) -> list[SnapshotMeta]:
    result = await session.execute(
        select(Snapshot)
        .where(Snapshot.tick >= from_tick, Snapshot.tick <= to_tick)
        .order_by(Snapshot.tick)  # type: ignore[arg-type]
        .limit(1000)
    )
    snaps = result.scalars().all()
    return [
        SnapshotMeta(id=s.id, tick=s.tick, file_path=s.file_path, size_bytes=s.size_bytes)
        for s in snaps
    ]


@router.get("/{snapshot_id}")
async def get_snapshot(
    snapshot_id: str,
    session: AsyncSession = Depends(get_session),
) -> dict[str, object]:
    result = await session.execute(select(Snapshot).where(Snapshot.id == snapshot_id))
    snap = result.scalars().first()
    if snap is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Snapshot not found")

    path = Path(snap.file_path).resolve()
    # Ensure the resolved path stays within the snapshots directory (path traversal protection).
    base_dir = Path("snapshots").resolve()
    try:
        path.relative_to(base_dir)
    except ValueError as exc:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN, detail="Invalid snapshot path"
        ) from exc
    if not path.exists():
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Snapshot file missing")

    return read_snapshot(path)


lineage_router = APIRouter(prefix="/lineage", tags=["lineage"])


@lineage_router.get("", response_model=list[LineageEventResponse])
async def list_lineage(
    bot_id: str | None = None,
    from_tick: int = Query(0, alias="from_tick"),
    to_tick: int = Query(999999999, alias="to_tick"),
    session: AsyncSession = Depends(get_session),
) -> list[LineageEventResponse]:
    stmt = select(LineageEvent).where(LineageEvent.tick >= from_tick, LineageEvent.tick <= to_tick)
    if bot_id is not None:
        stmt = stmt.where(LineageEvent.bot_id == bot_id)
    stmt = stmt.order_by(LineageEvent.tick).limit(10000)  # type: ignore[arg-type]

    result = await session.execute(stmt)
    events = result.scalars().all()
    return [
        LineageEventResponse(
            id=e.id,
            tick=e.tick,
            event_type=e.event_type,
            agent_id=e.agent_id,
            bot_id=e.bot_id,
            parent_agent_id=e.parent_agent_id,
            generation=e.generation,
            cause=e.cause,
        )
        for e in events
    ]
