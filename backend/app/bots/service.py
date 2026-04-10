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


async def get_latest_versions_bulk(
    bot_ids: list[str], session: AsyncSession
) -> dict[str, BotVersion]:
    """Fetch the latest BotVersion for each bot in a single query (avoids N+1)."""
    from sqlalchemy import func

    # Subquery: max version per bot_id.
    max_ver = (
        select(BotVersion.bot_id, func.max(BotVersion.version).label("max_version"))
        .where(BotVersion.bot_id.in_(bot_ids))  # type: ignore[union-attr]
        .group_by(BotVersion.bot_id)
        .subquery()
    )

    stmt = select(BotVersion).join(
        max_ver,
        (BotVersion.bot_id == max_ver.c.bot_id) & (BotVersion.version == max_ver.c.max_version),
    )
    result = await session.execute(stmt)
    versions = result.scalars().all()
    return {v.bot_id: v for v in versions}


async def publish_bot(bot: Bot, session: AsyncSession) -> None:
    bot.published = True
    session.add(bot)
    await session.commit()


async def delete_bot(bot: Bot, session: AsyncSession) -> None:
    """Delete a bot and all its versions."""
    # Delete versions first (FK constraint).
    versions = await session.execute(select(BotVersion).where(BotVersion.bot_id == bot.id))
    for v in versions.scalars().all():
        await session.delete(v)
    await session.delete(bot)
    await session.commit()
