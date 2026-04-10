"""Authentication service: password hashing and JWT token management."""

from __future__ import annotations

import logging
from datetime import UTC, datetime, timedelta

import jwt
from argon2 import PasswordHasher
from argon2.exceptions import VerifyMismatchError

from app.config import Settings

logger = logging.getLogger(__name__)

_hasher = PasswordHasher()


def hash_password(password: str) -> str:
    return _hasher.hash(password)


def verify_password(password: str, hashed: str) -> bool:
    try:
        return _hasher.verify(hashed, password)
    except VerifyMismatchError:
        return False


def create_token(user_id: str, settings: Settings) -> str:
    payload = {
        "sub": user_id,
        "exp": datetime.now(UTC) + timedelta(days=settings.jwt_expire_days),
        "iat": datetime.now(UTC),
    }
    return jwt.encode(payload, settings.jwt_secret, algorithm=settings.jwt_algorithm)


def decode_token(token: str, settings: Settings) -> str | None:
    """Decode a JWT and return the user_id (sub claim), or None if invalid."""
    try:
        payload = jwt.decode(token, settings.jwt_secret, algorithms=[settings.jwt_algorithm])
        return payload.get("sub")
    except jwt.ExpiredSignatureError:
        logger.debug("JWT token expired")
        return None
    except jwt.PyJWTError:
        logger.debug("JWT decode failed", exc_info=True)
        return None
