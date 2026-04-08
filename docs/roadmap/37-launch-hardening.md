# 37 — Launch hardening

**Status**: not started
**Started**: —
**Finished**: —

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
- [ ] Rate limits enforced and tested.
- [ ] Quotas enforced.
- [ ] Email verify + reset flows work end-to-end.
- [ ] Public docs site renders the spec.
- [ ] Backup script runs nightly in compose.
- [ ] **v1 is launchable** — all phases 0–7 marked done.
- [ ] `docs/roadmap/README.md` table updated to `done`.
