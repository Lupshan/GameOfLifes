"""Tests for the database models."""

from __future__ import annotations

from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession, async_sessionmaker, create_async_engine
from sqlmodel import SQLModel

from app.db.models import Bot, BotVersion, LineageEvent, Snapshot, User


async def test_user_round_trip() -> None:
    engine = create_async_engine("sqlite+aiosqlite://", echo=False)
    async with engine.begin() as conn:
        await conn.run_sync(SQLModel.metadata.create_all)

    factory = async_sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)
    async with factory() as session:
        user = User(email="test@example.com", password_hash="hashed")
        session.add(user)
        await session.commit()

        result = await session.execute(select(User).where(User.email == "test@example.com"))
        fetched = result.scalars().one()
        assert fetched.email == "test@example.com"
        assert fetched.password_hash == "hashed"
        assert fetched.id is not None

    await engine.dispose()


async def test_bot_with_version() -> None:
    engine = create_async_engine("sqlite+aiosqlite://", echo=False)
    async with engine.begin() as conn:
        await conn.run_sync(SQLModel.metadata.create_all)

    factory = async_sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)
    async with factory() as session:
        user = User(email="dev@example.com", password_hash="h")
        session.add(user)
        await session.commit()

        bot = Bot(name="walker", user_id=user.id)
        session.add(bot)
        await session.commit()

        version = BotVersion(
            bot_id=bot.id,
            source="fn main() { move(); }",
            bytecode=b"\x00\x01\x02",
            compile_ok=True,
        )
        session.add(version)
        await session.commit()

        result = await session.execute(select(BotVersion).where(BotVersion.bot_id == bot.id))
        fetched = result.scalars().one()
        assert fetched.source == "fn main() { move(); }"
        assert fetched.bytecode == b"\x00\x01\x02"
        assert fetched.compile_ok is True

    await engine.dispose()


async def test_snapshot_model() -> None:
    engine = create_async_engine("sqlite+aiosqlite://", echo=False)
    async with engine.begin() as conn:
        await conn.run_sync(SQLModel.metadata.create_all)

    factory = async_sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)
    async with factory() as session:
        snap = Snapshot(tick=42, file_path="/data/snapshots/tick_42.json.zst", size_bytes=1024)
        session.add(snap)
        await session.commit()

        result = await session.execute(select(Snapshot).where(Snapshot.tick == 42))
        fetched = result.scalars().one()
        assert fetched.file_path == "/data/snapshots/tick_42.json.zst"

    await engine.dispose()


async def test_lineage_event_model() -> None:
    engine = create_async_engine("sqlite+aiosqlite://", echo=False)
    async with engine.begin() as conn:
        await conn.run_sync(SQLModel.metadata.create_all)

    factory = async_sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)
    async with factory() as session:
        event = LineageEvent(
            tick=100,
            event_type="birth",
            agent_id=42,
            parent_agent_id=10,
            generation=3,
        )
        session.add(event)
        await session.commit()

        result = await session.execute(select(LineageEvent).where(LineageEvent.tick == 100))
        fetched = result.scalars().one()
        assert fetched.event_type == "birth"
        assert fetched.agent_id == 42

    await engine.dispose()
