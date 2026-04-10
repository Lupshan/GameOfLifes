"""Tests for the stats endpoint."""

from __future__ import annotations

from httpx import AsyncClient


async def test_stats_empty(client: AsyncClient) -> None:
    resp = await client.get("/stats?from=0&to=1000")
    assert resp.status_code == 200
    data = resp.json()
    assert data["total_births"] == 0
    assert data["total_deaths"] == 0
    assert data["buckets"] == []
