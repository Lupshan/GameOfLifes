# ADR 012 — Auth libraries: pyjwt + argon2-cffi

**Status**: accepted
**Date**: 2026-04-10

## Context

The backend needs JWT token creation/verification and password hashing. The roadmap originally specified `python-jose` and `passlib`.

## Decision

Use **pyjwt** (not python-jose) and **argon2-cffi** directly (not through passlib).

- `python-jose`: last release 2022, no Python 3.12+ testing, multiple open CVEs. `pyjwt` is actively maintained (monthly releases), smaller API surface, same functionality.
- `passlib`: semi-maintained (last release 2022), heavy abstraction over multiple hash algorithms. Since we only need Argon2, using `argon2-cffi` directly (the reference Python binding, actively maintained) is simpler and more secure.

## Consequences

- Fewer transitive dependencies.
- Direct Argon2 API: `PasswordHasher().hash(password)` and `.verify(hash, password)`.
- JWT: `jwt.encode()` / `jwt.decode()` with HS256.
- If we need additional hash algorithms later, we add them directly rather than through passlib.
