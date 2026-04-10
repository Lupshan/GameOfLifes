"""Tests for snapshot storage and REST endpoints."""

from __future__ import annotations

import json
from pathlib import Path

from httpx import AsyncClient

from app.snapshots.storage import prune_snapshots, read_snapshot, write_snapshot

# ---- Storage unit tests ----


def test_write_and_read_snapshot(tmp_path: Path) -> None:
    data = {"event": "snapshot", "tick": 42, "agents": [{"id": 1}]}
    path = tmp_path / "test.json.zst"
    size = write_snapshot(path, data)
    assert size > 0
    assert path.exists()

    restored = read_snapshot(path)
    assert restored["tick"] == 42
    assert len(restored["agents"]) == 1


def test_compression_ratio(tmp_path: Path) -> None:
    # A large snapshot should compress significantly.
    data = {"tick": 1, "agents": [{"id": i, "x": i, "y": i} for i in range(500)]}
    raw_size = len(json.dumps(data).encode())
    path = tmp_path / "big.json.zst"
    compressed_size = write_snapshot(path, data)
    assert compressed_size < raw_size


def test_prune_old_snapshots(tmp_path: Path) -> None:
    import os
    import time

    # Create a "new" file.
    new_file = tmp_path / "2026/04/10/tick_000100.json.zst"
    new_file.parent.mkdir(parents=True)
    write_snapshot(new_file, {"tick": 100})

    # Create an "old" file and backdate it.
    old_file = tmp_path / "2026/03/01/tick_000001.json.zst"
    old_file.parent.mkdir(parents=True)
    write_snapshot(old_file, {"tick": 1})
    old_time = time.time() - (30 * 86400)  # 30 days ago
    os.utime(old_file, (old_time, old_time))

    deleted = prune_snapshots(tmp_path, keep_days=7)
    assert deleted == 1
    assert not old_file.exists()
    assert new_file.exists()


# ---- REST endpoint tests ----


async def test_list_snapshots_empty(client: AsyncClient) -> None:
    resp = await client.get("/snapshots?from=0&to=100")
    assert resp.status_code == 200
    assert resp.json() == []


async def test_get_snapshot_not_found(client: AsyncClient) -> None:
    resp = await client.get("/snapshots/nonexistent")
    assert resp.status_code == 404


async def test_lineage_empty(client: AsyncClient) -> None:
    resp = await client.get("/lineage?from_tick=0&to_tick=100")
    assert resp.status_code == 200
    assert resp.json() == []
