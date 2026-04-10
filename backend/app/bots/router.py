"""Bot CRUD endpoints."""

from __future__ import annotations

import json

from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from app.bots.schemas import BotCreateRequest, BotDetailResponse, BotResponse, CompileErrorResponse
from app.bots.service import (
    create_bot,
    get_bot,
    get_latest_version,
    get_latest_versions_bulk,
    list_user_bots,
    publish_bot,
)
from app.db.engine import get_session
from app.db.models import User
from app.deps import current_user

MAX_BOTS_PER_USER = 10  # Hard cap to prevent abuse; one user shouldn't flood the world.
MAX_SOURCE_SIZE = 10240  # 10 KB — keeps compilation fast and bytecode bounded.

router = APIRouter(prefix="/bots", tags=["bots"])


def _bot_response(bot: object, compile_ok: bool, compile_errors: str | None) -> BotResponse:
    from app.db.models import Bot as BotModel

    assert isinstance(bot, BotModel)
    errors = None
    if compile_errors:
        raw = json.loads(compile_errors)
        errors = [CompileErrorResponse(**e) for e in raw]
    return BotResponse(
        id=bot.id,
        name=bot.name,
        published=bot.published,
        compile_ok=compile_ok,
        compile_errors=errors,
    )


@router.post("", response_model=BotResponse, status_code=status.HTTP_201_CREATED)
async def create(
    body: BotCreateRequest,
    user: User = Depends(current_user),
    session: AsyncSession = Depends(get_session),
) -> BotResponse:
    # Quota: max bots per user.
    existing = await list_user_bots(user.id, session)
    if len(existing) >= MAX_BOTS_PER_USER:
        raise HTTPException(
            status_code=status.HTTP_429_TOO_MANY_REQUESTS, detail="Bot limit reached"
        )

    # Source size limit.
    if len(body.source) > MAX_SOURCE_SIZE:
        raise HTTPException(
            status_code=status.HTTP_413_REQUEST_ENTITY_TOO_LARGE, detail="Source too large"
        )

    bot, version = await create_bot(body.name, body.source, user.id, session)
    return _bot_response(bot, version.compile_ok, version.compile_errors)


@router.get("", response_model=list[BotResponse])
async def list_bots(
    user: User = Depends(current_user),
    session: AsyncSession = Depends(get_session),
) -> list[BotResponse]:
    bots = await list_user_bots(user.id, session)
    if not bots:
        return []
    versions = await get_latest_versions_bulk([b.id for b in bots], session)
    results: list[BotResponse] = []
    for bot in bots:
        version = versions.get(bot.id)
        compile_ok = version.compile_ok if version else False
        compile_errors = version.compile_errors if version else None
        results.append(_bot_response(bot, compile_ok, compile_errors))
    return results


@router.get("/{bot_id}", response_model=BotDetailResponse)
async def get_bot_detail(
    bot_id: str,
    user: User = Depends(current_user),
    session: AsyncSession = Depends(get_session),
) -> BotDetailResponse:
    bot = await get_bot(bot_id, session)
    if bot is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Bot not found")
    if bot.user_id != user.id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Not your bot")
    version = await get_latest_version(bot_id, session)
    source = version.source if version else ""
    compile_ok = version.compile_ok if version else False
    compile_errors = version.compile_errors if version else None
    errors = None
    if compile_errors:
        raw = json.loads(compile_errors)
        errors = [CompileErrorResponse(**e) for e in raw]
    return BotDetailResponse(
        id=bot.id,
        name=bot.name,
        published=bot.published,
        compile_ok=compile_ok,
        compile_errors=errors,
        source=source,
    )


@router.post("/{bot_id}/publish", response_model=BotResponse)
async def publish(
    bot_id: str,
    user: User = Depends(current_user),
    session: AsyncSession = Depends(get_session),
) -> BotResponse:
    from sqlalchemy import select as sa_select

    from app.db.models import Bot as BotModel

    # Lock the bot row to prevent concurrent publish races.
    result = await session.execute(
        sa_select(BotModel).where(BotModel.id == bot_id).with_for_update()
    )
    bot = result.scalars().first()
    if bot is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Bot not found")
    if bot.user_id != user.id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Not your bot")
    version = await get_latest_version(bot_id, session)
    if version is None or not version.compile_ok:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST, detail="Bot has compile errors"
        )
    await publish_bot(bot, session)
    return _bot_response(bot, version.compile_ok, version.compile_errors)
