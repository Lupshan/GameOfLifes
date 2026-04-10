"""Snapshot persistence service."""

from __future__ import annotations

import logging
from pathlib import Path

from sqlalchemy.ext.asyncio import AsyncSession

from app.db.models import Snapshot
from app.snapshots.storage import prune_snapshots, snapshot_path, write_snapshot

logger = logging.getLogger(__name__)


async def store_snapshot(
    data: dict[str, object],
    tick: int,
    base_dir: Path,
    session: AsyncSession,
) -> Snapshot:
    """Store a snapshot to disk and record it in the database."""
    path = snapshot_path(base_dir, tick)
    size = write_snapshot(path, data)

    snap = Snapshot(tick=tick, file_path=str(path), size_bytes=size)
    session.add(snap)
    await session.commit()
    await session.refresh(snap)

    logger.debug("Stored snapshot tick=%d size=%d path=%s", tick, size, path)
    return snap


async def prune(base_dir: Path, keep_days: int) -> int:
    """Prune old snapshots from disk."""
    deleted = prune_snapshots(base_dir, keep_days)
    if deleted > 0:
        logger.info("Pruned %d old snapshots", deleted)
    return deleted
