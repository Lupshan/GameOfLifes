# 28 — Auth + bot management UI

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Frontend pages for the auth flow and a simple "my bots" dashboard. After this step, a user can sign up, log in, and see (empty) bot list — but not yet submit bots (step 30).

## Scope
- `/signup`, `/login`, `/logout` pages.
- `/me/bots` dashboard (auth-gated route, lists user's bots).
- Auth token stored in `httpOnly` cookie (set by backend on login).
- Auth store (Svelte writable) tracking current user.
- Form validation client-side + server-side errors surfaced inline.

## Non-goals
- No bot editing yet (step 29 + 30).
- No password reset.
- No profile customization.

## Technical notes
- Backend gains a `Set-Cookie` path on login (HttpOnly, Secure, SameSite=Lax).
- SvelteKit `hooks.server.ts` reads the cookie and populates `event.locals.user`.

## Definition of Done
- [ ] Signup → login → dashboard flow works end to end.
- [ ] Logout clears cookie + state.
- [ ] Auth-gated route redirects unauth users.
- [ ] `docs/roadmap/README.md` table updated to `done`.
