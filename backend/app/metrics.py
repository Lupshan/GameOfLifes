"""Prometheus metrics for the backend."""

from __future__ import annotations

from prometheus_client import Counter, Gauge, Histogram, generate_latest
from starlette.requests import Request
from starlette.responses import Response

# Metrics.
REQUEST_COUNT = Counter(
    "gol_http_requests_total", "Total HTTP requests", ["method", "path", "status"]
)
REQUEST_LATENCY = Histogram(
    "gol_http_request_duration_seconds", "Request latency", ["method", "path"]
)
WS_CONNECTIONS = Gauge("gol_ws_connections", "Active WebSocket connections")
COMPILE_ERRORS = Counter("gol_compile_errors_total", "Total compilation errors")
AGENT_COUNT = Gauge("gol_agent_count", "Current agent count in simulation")
TICK_RATE = Gauge("gol_tick_rate_hz", "Current tick rate")


async def metrics_endpoint(_request: Request) -> Response:
    """Prometheus metrics endpoint."""
    return Response(
        content=generate_latest(), media_type="text/plain; version=0.0.4; charset=utf-8"
    )
