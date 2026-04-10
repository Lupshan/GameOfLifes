"""Private simulation endpoints."""

from __future__ import annotations

import asyncio
import uuid
from datetime import UTC, datetime

from fastapi import APIRouter, Depends, HTTPException, status
from pydantic import BaseModel
from sqlalchemy.ext.asyncio import AsyncSession

from app.db.engine import get_session
from app.db.models import User
from app.deps import current_user

router = APIRouter(prefix="/sims", tags=["sims"])

MAX_SIMS_PER_USER = 3  # Prevent a single user from monopolizing engine subprocesses.
MAX_TICKS_PER_SIM = 10000  # ~2 min of sim time at 100 ticks/s; keeps shared infra fair.


class SimCreateRequest(BaseModel):
    name: str
    config: dict[str, object] = {}
    bot_ids: list[str] = []


class SimResponse(BaseModel):
    id: str
    name: str
    status: str
    max_ticks: int
    created_at: str


# In-memory sim registry — intentionally ephemeral for v1.
# Sims are lightweight, short-lived proxies to engine subprocesses.
# They are recreated on demand; losing them on restart is acceptable.
# Persist to DB if sims need to survive restarts (tracked in audit B6).
_sims: dict[str, dict[str, object]] = {}
_sims_lock = asyncio.Lock()  # Prevents TOCTOU on quota check + insert.


@router.post("", response_model=SimResponse, status_code=status.HTTP_201_CREATED)
async def create_sim(
    body: SimCreateRequest,
    user: User = Depends(current_user),
    session: AsyncSession = Depends(get_session),
) -> SimResponse:
    async with _sims_lock:
        # Quota check under lock to prevent TOCTOU race.
        user_sims = [s for s in _sims.values() if s.get("user_id") == user.id]
        if len(user_sims) >= MAX_SIMS_PER_USER:
            raise HTTPException(
                status_code=status.HTTP_429_TOO_MANY_REQUESTS,
                detail=f"Max {MAX_SIMS_PER_USER} concurrent sims",
            )

        sim_id = uuid.uuid4().hex
        now = datetime.now(UTC).isoformat()
        sim = {
            "id": sim_id,
            "name": body.name,
            "user_id": user.id,
            "status": "running",
            "config": body.config,
            "bot_ids": body.bot_ids,
            "max_ticks": min(body.config.get("max_ticks", MAX_TICKS_PER_SIM), MAX_TICKS_PER_SIM),  # type: ignore[arg-type]
            "created_at": now,
        }
        _sims[sim_id] = sim

    return SimResponse(
        id=sim_id,
        name=body.name,
        status="running",
        max_ticks=sim["max_ticks"],  # type: ignore[arg-type]
        created_at=now,
    )


@router.get("", response_model=list[SimResponse])
async def list_sims(
    user: User = Depends(current_user),
) -> list[SimResponse]:
    user_sims = [s for s in _sims.values() if s.get("user_id") == user.id]
    return [
        SimResponse(
            id=s["id"],  # type: ignore[arg-type]
            name=s["name"],  # type: ignore[arg-type]
            status=s["status"],  # type: ignore[arg-type]
            max_ticks=s["max_ticks"],  # type: ignore[arg-type]
            created_at=s["created_at"],  # type: ignore[arg-type]
        )
        for s in user_sims
    ]


@router.get("/{sim_id}", response_model=SimResponse)
async def get_sim(
    sim_id: str,
    user: User = Depends(current_user),
) -> SimResponse:
    sim = _sims.get(sim_id)
    if sim is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Sim not found")
    if sim.get("user_id") != user.id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Not your sim")
    return SimResponse(
        id=sim["id"],  # type: ignore[arg-type]
        name=sim["name"],  # type: ignore[arg-type]
        status=sim["status"],  # type: ignore[arg-type]
        max_ticks=sim["max_ticks"],  # type: ignore[arg-type]
        created_at=sim["created_at"],  # type: ignore[arg-type]
    )


@router.delete("/{sim_id}")
async def delete_sim(
    sim_id: str,
    user: User = Depends(current_user),
) -> dict[str, str]:
    sim = _sims.get(sim_id)
    if sim is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Sim not found")
    if sim.get("user_id") != user.id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Not your sim")
    del _sims[sim_id]
    return {"status": "deleted"}
