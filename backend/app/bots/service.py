"""Bot compilation and storage service."""

from __future__ import annotations

import json

from compiler import compile as compile_source
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.db.models import Bot, BotVersion


async def create_bot(
    name: str, source: str, user_id: str, session: AsyncSession
) -> tuple[Bot, BotVersion]:
    """Create a bot and its first version with compiled bytecode."""
    bot = Bot(name=name, user_id=user_id)
    session.add(bot)
    await session.commit()
    await session.refresh(bot)

    result = compile_source(source)

    errors_json: str | None = None
    if not result.ok:
        errors_json = json.dumps(
            [{"message": e.message, "line": e.line, "col": e.col} for e in result.errors]
        )

    version = BotVersion(
        bot_id=bot.id,
        source=source,
        bytecode=result.bytecode,
        compile_ok=result.ok,
        compile_errors=errors_json,
    )
    session.add(version)
    await session.commit()
    await session.refresh(version)

    return bot, version


async def list_user_bots(user_id: str, session: AsyncSession) -> list[Bot]:
    result = await session.execute(select(Bot).where(Bot.user_id == user_id))
    return list(result.scalars().all())


async def get_bot(bot_id: str, session: AsyncSession) -> Bot | None:
    result = await session.execute(select(Bot).where(Bot.id == bot_id))
    return result.scalars().first()


async def get_latest_version(bot_id: str, session: AsyncSession) -> BotVersion | None:
    result = await session.execute(
        select(BotVersion).where(BotVersion.bot_id == bot_id).order_by(BotVersion.version.desc())  # type: ignore[union-attr]
    )
    return result.scalars().first()


async def publish_bot(bot: Bot, session: AsyncSession) -> None:
    bot.published = True
    session.add(bot)
    await session.commit()
