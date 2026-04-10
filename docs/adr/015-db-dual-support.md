# ADR 015 — SQLite dev / Postgres prod dual support

**Status**: accepted
**Date**: 2026-04-10

## Context

The backend needs a relational database for users, bots, snapshots, and lineage events. Development should be zero-setup (no Docker required for local dev), while production needs a robust concurrent database.

## Decision

**SQLite for development, Postgres for production.** Both use the same SQLModel/SQLAlchemy models and Alembic migrations. The database URL is configured via environment variable (`GOL_DATABASE_URL`).

- **Dev default**: `sqlite+aiosqlite:///./gameoflifes.db` — zero setup, single file.
- **Prod**: `postgresql+asyncpg://user:pass@host/dbname` — set via env.
- **Tests**: `sqlite+aiosqlite://` (in-memory) — fast, isolated, disposable.

**ORM**: SQLModel (combines Pydantic validation + SQLAlchemy ORM in one class). Avoids duplicate model/schema definitions.

**Migrations**: Alembic with autogenerate from SQLModel metadata. Migrations are tested against SQLite locally.

## Consequences

- Developers need only Python + uv to run the backend locally. No Docker, no Postgres install.
- SQLite limitations (no concurrent writes, no JSONB, no full-text) are acceptable for local dev — the backend handles one developer at a time.
- Alembic migrations must avoid Postgres-specific SQL (e.g., no `ALTER TYPE`). If needed, use `op.execute()` with dialect checks.
- The async drivers (`aiosqlite` for SQLite, `asyncpg` for Postgres) are both community-driven and well-maintained.
