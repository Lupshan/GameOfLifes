"""Snapshot disk storage with zstd compression."""

from __future__ import annotations

import json
from datetime import UTC, datetime
from pathlib import Path

import zstandard as zstd


def snapshot_path(base_dir: Path, tick: int) -> Path:
    """Generate storage path: base/YYYY/MM/DD/tick_NNNNNN.json.zst."""
    now = datetime.now(UTC)
    date_dir = base_dir / now.strftime("%Y/%m/%d")
    date_dir.mkdir(parents=True, exist_ok=True)
    return date_dir / f"tick_{tick:06d}.json.zst"


def write_snapshot(path: Path, data: dict[str, object]) -> int:
    """Compress and write a snapshot to disk. Returns size in bytes."""
    raw = json.dumps(data).encode("utf-8")
    compressor = zstd.ZstdCompressor(level=3)
    compressed = compressor.compress(raw)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(compressed)
    return len(compressed)


def read_snapshot(path: Path) -> dict[str, object]:
    """Read and decompress a snapshot from disk."""
    compressed = path.read_bytes()
    decompressor = zstd.ZstdDecompressor()
    raw = decompressor.decompress(compressed)
    return json.loads(raw)  # type: ignore[no-any-return]


def prune_snapshots(base_dir: Path, keep_days: int) -> int:
    """Delete snapshots older than keep_days. Returns number of files deleted."""
    if not base_dir.exists():
        return 0

    cutoff = datetime.now(UTC).timestamp() - (keep_days * 86400)
    deleted = 0
    for zst_file in base_dir.rglob("*.json.zst"):
        if zst_file.stat().st_mtime < cutoff:
            zst_file.unlink()
            deleted += 1

    # Clean up empty directories.
    for d in sorted(base_dir.rglob("*"), reverse=True):
        if d.is_dir() and not any(d.iterdir()):
            d.rmdir()

    return deleted
