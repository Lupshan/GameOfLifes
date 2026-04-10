"""Rate limiting middleware using slowapi."""

from __future__ import annotations

import os

from slowapi import Limiter
from slowapi.util import get_remote_address

# Disable rate limiting in tests.
_enabled = os.environ.get("GOL_RATE_LIMIT", "1") != "0"

limiter = Limiter(
    key_func=get_remote_address,
    enabled=_enabled,
    default_limits=["60/minute"],  # Global: 60 requests/min per IP.
    # Note: per-endpoint auth limits (B3) would ideally be 5/min, but slowapi's
    # decorator conflicts with FastAPI body parsing. The global 60/min limit
    # still provides brute-force protection; tighten it or add WAF rules for
    # /auth/ in production.
)
