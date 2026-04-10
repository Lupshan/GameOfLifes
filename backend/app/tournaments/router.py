"""Tournament endpoints: leaderboard and scoring."""

from __future__ import annotations

import uuid
from datetime import UTC, datetime

from fastapi import APIRouter, Depends, HTTPException, status
from pydantic import BaseModel

from app.db.models import User
from app.deps import current_user

router = APIRouter(prefix="/tournaments", tags=["tournaments"])


class TournamentCreateRequest(BaseModel):
    name: str
    description: str = ""
    max_ticks: int = 1000
    scoring: str = "energy"  # "energy" | "population" | "survival"


class TournamentResponse(BaseModel):
    id: str
    name: str
    description: str
    status: str
    max_ticks: int
    scoring: str
    created_at: str
    leaderboard: list[dict[str, object]]


# In-memory tournament registry — intentionally ephemeral for v1.
# Tournaments are recreated manually; losing them on restart is acceptable.
# Persist to DB if tournaments need to survive restarts (tracked in audit B6).
_tournaments: dict[str, dict[str, object]] = {}


@router.post("", response_model=TournamentResponse, status_code=status.HTTP_201_CREATED)
async def create_tournament(
    body: TournamentCreateRequest,
    user: User = Depends(current_user),
) -> TournamentResponse:
    t_id = uuid.uuid4().hex
    now = datetime.now(UTC).isoformat()
    t = {
        "id": t_id,
        "name": body.name,
        "description": body.description,
        "status": "open",
        "max_ticks": body.max_ticks,
        "scoring": body.scoring,
        "created_at": now,
        "creator_id": user.id,
        "leaderboard": [],
    }
    _tournaments[t_id] = t
    return TournamentResponse(**{k: v for k, v in t.items() if k != "creator_id"})  # type: ignore[arg-type]


@router.get("", response_model=list[TournamentResponse])
async def list_tournaments() -> list[TournamentResponse]:
    return [
        TournamentResponse(**{k: v for k, v in t.items() if k != "creator_id"})  # type: ignore[arg-type]
        for t in _tournaments.values()
    ]


@router.get("/{tournament_id}", response_model=TournamentResponse)
async def get_tournament(tournament_id: str) -> TournamentResponse:
    t = _tournaments.get(tournament_id)
    if t is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Tournament not found")
    return TournamentResponse(**{k: v for k, v in t.items() if k != "creator_id"})  # type: ignore[arg-type]
