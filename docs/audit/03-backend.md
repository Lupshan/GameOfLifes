# Partie 3 — Backend (Python/FastAPI)

**Priorité** : P1 — fonctionnel, performance, validation.

## Tâches

- [ ] **B6. Sims/tournaments perdues au restart**
  - Fichiers : `backend/app/sims/router.py`, `backend/app/tournaments/router.py`
  - Problème : stockage in-memory (`dict`), perdu au redémarrage
  - Fix : soit persister en DB (ajouter modèles SQLModel), soit documenter explicitement comme éphémère dans l'API

- [ ] **C1. Codegen — suppression aveugle de 5 bytes pour my_trait**
  - Fichier : `bytecode/compiler/codegen.py` (ligne ~353)
  - Problème : `del em.code[-5:]` suppose que le dernier émis était PUSH_INT
  - Fix : ne pas émettre les args pour `my_trait` via `_emit_expr`, gérer directement dans le case `my_trait`

- [ ] **C2. N+1 query sur la liste de bots**
  - Fichier : `backend/app/bots/router.py` (endpoint `list_bots`)
  - Problème : 1 query par bot pour `get_latest_version()`
  - Fix : requête SQL avec `JOIN` ou subquery `MAX(version)` groupée par `bot_id`

- [ ] **C3. Stats endpoint charge tout en mémoire**
  - Fichier : `backend/app/stats/router.py`
  - Problème : `select(LineageEvent)` sans agrégation SQL, tout chargé en Python
  - Fix : `SELECT tick / bucket_size AS bucket, event_type, COUNT(*) GROUP BY bucket, event_type` côté SQL

- [ ] **C6. Pas de validation email**
  - Fichier : `backend/app/auth/schemas.py`
  - Problème : `email: str` sans validation format
  - Fix : `email: EmailStr` (pydantic) — ajouter `pydantic[email]` aux deps

- [ ] **C7. Pas de validation nom de bot**
  - Fichier : `backend/app/bots/schemas.py`
  - Fix : `name: str = Field(min_length=1, max_length=100)`

- [ ] **C8. Race condition sur publish**
  - Fichier : `backend/app/bots/router.py` (endpoint `publish`)
  - Fix : wrap dans une transaction, ou `SELECT ... FOR UPDATE`

- [ ] **C9. Rate limiting inopérant**
  - Fichiers : `backend/app/middleware/rate_limit.py`, `backend/app/auth/router.py`, `backend/app/bots/router.py`
  - Problème : le limiter existe mais aucune route n'a `@limiter.limit()`
  - Fix : remettre les décorateurs. Le bug initial (422 avec `request: Request`) se résout en utilisant `slowapi.middleware.SlowAPIMiddleware` au lieu des décorateurs per-route

- [ ] **C10. Pruning snapshots jamais appelé**
  - Fichier : `backend/app/snapshots/service.py`
  - Fix : ajouter une tâche périodique. Options :
    - `asyncio.create_task` dans le lifespan qui appelle `prune()` toutes les heures
    - Ou un cron externe via compose

- [ ] **M3. test_smoke.py placeholder**
  - Fichiers : `backend/tests/test_smoke.py`, `bytecode/tests/test_smoke.py`
  - Fix : supprimer les deux fichiers, les vrais tests existent déjà

- [ ] **M4. Magic numbers non documentés**
  - Fichiers : multiples
  - Fix : ajouter des commentaires `// Why 10: ...` sur `INTRINSIC_COST`, `MAX_BOTS_PER_USER`, etc.
