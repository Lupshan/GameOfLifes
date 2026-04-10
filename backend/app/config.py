"""Application settings loaded from environment variables."""

from __future__ import annotations

import warnings

from pydantic_settings import BaseSettings

_DEFAULT_JWT_SECRET = "change-me-in-production"


class Settings(BaseSettings):
    app_name: str = "GameOfLifes"
    debug: bool = False
    host: str = "0.0.0.0"
    port: int = 8000
    database_url: str = "sqlite+aiosqlite:///./gameoflifes.db"
    jwt_secret: str = _DEFAULT_JWT_SECRET
    jwt_algorithm: str = "HS256"
    jwt_expire_days: int = 7

    model_config = {"env_prefix": "GOL_"}


def get_settings() -> Settings:
    settings = Settings()
    if not settings.debug and settings.jwt_secret == _DEFAULT_JWT_SECRET:
        raise RuntimeError(
            "CRITICAL: GOL_JWT_SECRET is still the default value. "
            "Set a strong secret via environment variable before running in production."
        )
    if settings.debug and settings.jwt_secret == _DEFAULT_JWT_SECRET:
        warnings.warn("Using default JWT secret — acceptable in debug mode only.", stacklevel=2)
    return settings
