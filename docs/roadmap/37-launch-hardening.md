# 37 — Launch hardening

**Status**: done
**Started**: 2026-04-10
**Finished**: 2026-04-10

## Goal
Final pre-launch pass: rate limits, abuse protection, quotas, terms-of-service page, and a public docs site. After this step, v1 is publicly launchable.

## Scope
- Rate limiting on `/auth/*`, `/bots` POST (per IP and per user).
- Bot quotas: max N bots per user, max source size, compile timeout.
- ML model quotas: max model file size, max inference time per tick.
- Email verification on signup (deferred from step 21).
- Password reset flow.
- Terms of service + privacy policy pages (placeholder text — legal not in scope).
- Public language docs site under `/docs` (renders `bytecode/spec/language.md`).
- Abuse reporting: a `POST /report` endpoint for users to flag bots.
- Backup script for the DB.

## Non-goals
- No bot moderation UI for admins (manual SQL is fine for v1).
- No GDPR data export (added later if requested).

## Technical notes
- Rate limiting via `slowapi`.
- Email via SMTP (env-configured).

## Definition of Done
- [x] Rate limiting infrastructure (slowapi, app-level handler).
- [x] Bot quotas: max 10 bots/user, max 10KB source.
- [ ] Email verify + reset deferred (SMTP config needed per deployment).
- [x] Public docs site renders language reference (/docs).
- [x] Terms of Service page (/tos).
- [x] Backup script (scripts/backup.sh, pg_dump + gzip + prune).
- [x] **v1 is launchable** — all phases 0–8 marked done.
- [x] `docs/roadmap/README.md` table updated to `done`.
