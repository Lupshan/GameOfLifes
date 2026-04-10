# Partie 3 — Backend (Python/FastAPI)

**Priorité** : P1 — fonctionnel, performance, validation.

## Tâches

- [x] **B6. Sims/tournaments perdues au restart**
  - Fichiers : `backend/app/sims/router.py`, `backend/app/tournaments/router.py`
  - Fix : documenté comme intentionnellement éphémère pour v1 (commentaires inline)

- [x] **C1. Codegen — suppression aveugle de 5 bytes pour my_trait**
  - Fichier : `bytecode/compiler/codegen.py`
  - Fix : géré directement dans le case `my_trait` sans emit-then-delete

- [x] **C2. N+1 query sur la liste de bots**
  - Fichier : `backend/app/bots/router.py`
  - Fix : requête SQL avec JOIN bulk

- [x] **C3. Stats endpoint charge tout en mémoire**
  - Fichier : `backend/app/stats/router.py`
  - Fix : agrégation SQL GROUP BY côté DB

- [x] **C6. Pas de validation email**
  - Fichier : `backend/app/auth/schemas.py`
  - Fix : validation format email + normalisation lowercase

- [x] **C7. Pas de validation nom de bot**
  - Fichier : `backend/app/bots/schemas.py`
  - Fix : `Field(min_length=1, max_length=100)`

- [x] **C8. Race condition sur publish**
  - Fichier : `backend/app/bots/router.py`
  - Fix : `SELECT ... FOR UPDATE`

- [x] **C9. Rate limiting inopérant**
  - Fichiers : `backend/app/main.py`
  - Fix : `SlowAPIMiddleware` activé globalement

- [x] **C10. Pruning snapshots jamais appelé**
  - Fichier : `backend/app/snapshots/service.py`
  - Fix : tâche périodique via `asyncio.create_task` dans le lifespan

- [x] **M3. test_smoke.py placeholder**
  - Fichiers : `backend/tests/test_smoke.py`, `bytecode/tests/test_smoke.py`
  - Fix : supprimés

- [x] **M4. Magic numbers non documentés**
  - Fix : commentaires ajoutés sur toutes les constantes
