# 21 — User auth

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Add account management so users can sign up, log in, and obtain a token used to authenticate later bot submissions.

## Scope
- `POST /auth/signup` (email + password).
- `POST /auth/login` returning JWT.
- `GET /auth/me` returning current user (auth required).
- Password hashing with `argon2` (via `passlib`).
- JWT signed with HS256, secret from env.
- FastAPI dependency `current_user` for protected routes.
- Tests: signup happy path, duplicate email, login wrong password, token round-trip.

## Non-goals
- No OAuth / social login.
- No email verification (deferred to step 37 launch hardening).
- No password reset flow yet.

## Technical notes
- JWT lifetime: 7 days, refresh deferred.
- Rate limiting deferred to step 37.
- Strict pyright; no `Any` in auth code paths.

## Definition of Done
- [x] All endpoints implemented + tested (signup, login, me — 12 tests).
- [x] Argon2 password hashing (argon2-cffi direct, ADR 012).
- [x] `current_user` dependency works (JWT Bearer → DB lookup).
- [x] `docs/roadmap/README.md` table updated to `done`.
