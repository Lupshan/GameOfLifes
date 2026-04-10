"""Shared FastAPI dependencies."""

from __future__ import annotations

from fastapi import Cookie, Depends, HTTPException, status
from fastapi.security import HTTPAuthorizationCredentials, HTTPBearer
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.auth.service import decode_token
from app.config import Settings, get_settings
from app.db.engine import get_session
from app.db.models import User

_bearer = HTTPBearer(auto_error=False)

_COOKIE_NAME = "gol_token"


async def current_user(
    credentials: HTTPAuthorizationCredentials | None = Depends(_bearer),
    gol_token: str | None = Cookie(default=None),
    session: AsyncSession = Depends(get_session),
    settings: Settings = Depends(get_settings),
) -> User:
    # Prefer Bearer header (API clients), fall back to httpOnly cookie (browsers).
    token: str | None = None
    if credentials is not None:
        token = credentials.credentials
    elif gol_token is not None:
        token = gol_token

    if token is None:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Not authenticated")

    user_id = decode_token(token, settings)
    if user_id is None:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid token")

    result = await session.execute(select(User).where(User.id == user_id))
    user = result.scalars().first()
    if user is None:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="User not found")

    return user
