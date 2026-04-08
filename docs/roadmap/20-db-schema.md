# 20 — DB schema + migrations

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Define the persistent data model and a migration tool so schema evolution is tracked from day one.

## Scope
- Tables: `users`, `bots` (source + bytecode + author + status), `bot_versions`, `snapshots` (metadata + path), `lineage_events`.
- ORM models in `backend/app/db/models.py` (SQLModel).
- `alembic` set up for migrations; initial migration generated.
- Local dev DB: SQLite. Prod DB: Postgres (config-driven, same models).
- Test fixtures spinning up an in-memory SQLite DB per test.

## Non-goals
- No data seeding scripts beyond a tiny dev fixture.
- No read replicas, no sharding.

## Technical notes
- ADR `010-db-choice.md` documenting SQLite/Postgres dual support and the trade-off.
- All FK relationships explicit; no orphan rows allowed.
- `bytecode` column is binary blob (the `.gob` file content).

## Definition of Done
- [ ] Models defined and migration generated.
- [ ] `alembic upgrade head` works against fresh SQLite + Postgres.
- [ ] Tests cover model round-trip via fixtures.
- [ ] ADR 010 written.
- [ ] `docs/roadmap/README.md` table updated to `done`.
