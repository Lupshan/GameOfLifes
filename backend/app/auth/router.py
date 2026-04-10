"""Auth endpoints: signup, login, logout, me.

Tokens are delivered via httpOnly cookies (Secure, SameSite=Lax) to mitigate
XSS token theft.  The JSON body still echoes the token for non-browser clients.
"""

from __future__ import annotations

from fastapi import APIRouter, Depends, HTTPException, Response, status
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.auth.schemas import LoginRequest, SignupRequest, TokenResponse, UserResponse
from app.auth.service import create_token, hash_password, verify_password
from app.config import Settings, get_settings
from app.db.engine import get_session
from app.db.models import User
from app.deps import current_user

router = APIRouter(prefix="/auth", tags=["auth"])

_COOKIE_NAME = "gol_token"
_COOKIE_MAX_AGE = 7 * 24 * 3600  # 7 days, matches jwt_expire_days


def _set_token_cookie(response: Response, token: str, settings: Settings) -> None:
    response.set_cookie(
        key=_COOKIE_NAME,
        value=token,
        httponly=True,
        secure=not settings.debug,
        samesite="lax",
        max_age=_COOKIE_MAX_AGE,
        path="/",
    )


@router.post("/signup", response_model=TokenResponse, status_code=status.HTTP_201_CREATED)
async def signup(
    body: SignupRequest,
    response: Response,
    session: AsyncSession = Depends(get_session),
    settings: Settings = Depends(get_settings),
) -> TokenResponse:
    result = await session.execute(select(User).where(User.email == body.email))
    if result.scalars().first() is not None:
        raise HTTPException(status_code=status.HTTP_409_CONFLICT, detail="Email already registered")

    user = User(email=body.email, password_hash=hash_password(body.password))
    session.add(user)
    await session.commit()
    await session.refresh(user)

    token = create_token(user.id, settings)
    _set_token_cookie(response, token, settings)
    return TokenResponse(access_token=token)


@router.post("/login", response_model=TokenResponse)
async def login(
    body: LoginRequest,
    response: Response,
    session: AsyncSession = Depends(get_session),
    settings: Settings = Depends(get_settings),
) -> TokenResponse:
    result = await session.execute(select(User).where(User.email == body.email))
    user = result.scalars().first()
    if user is None or not verify_password(body.password, user.password_hash):
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid credentials")

    token = create_token(user.id, settings)
    _set_token_cookie(response, token, settings)
    return TokenResponse(access_token=token)


@router.post("/logout")
async def logout(response: Response) -> dict[str, str]:
    response.delete_cookie(key=_COOKIE_NAME, path="/")
    return {"status": "ok"}


@router.get("/me", response_model=UserResponse)
async def me(user: User = Depends(current_user)) -> UserResponse:
    return UserResponse(id=user.id, email=user.email)
