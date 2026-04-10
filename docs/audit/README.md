# Audit de code — GameOfLifes

**Date** : 2026-04-10
**Scope** : tout le code produit (40 steps de roadmap)
**Total** : 36 issues identifiées (9 bloquantes, 15 à corriger, 12 mineures)

## Fichiers par partie

| Partie | Fichier | Issues | Priorité |
|--------|---------|--------|----------|
| 1. Sécurité | [01-securite.md](01-securite.md) | B5, B7, B8, B9, C14, M8, M12 | **P0 — avant tout déploiement** |
| 2. Runtime engine (C++) | [02-runtime-engine.md](02-runtime-engine.md) | B1, B2, B3, B4, C4, C5, C13, C15, M10, M11 | **P1 — bugs et UB** |
| 3. Backend (Python) | [03-backend.md](03-backend.md) | B6, C1, C2, C3, C6, C7, C8, C9, C10, M3, M4 | **P1 — fonctionnel + perf** |
| 4. Frontend (Svelte) | [04-frontend.md](04-frontend.md) | C11, C12, M2, M5, M6, M7, M9 | **P2 — robustesse + hygiène** |
| 5. Tests manquants | [05-tests.md](05-tests.md) | M1, M2 | **P2 — couverture** |

## Ordre de fix recommandé

```
P0 : 01-securite.md        (~2h)  — failles critiques, zéro compromis
P1 : 02-runtime-engine.md  (~3h)  — crashes et UB dans le hot path
P1 : 03-backend.md         (~3h)  — N+1, rate limiting, validation
P2 : 04-frontend.md        (~2h)  — WS backoff, types, headers
P2 : 05-tests.md           (~3h)  — couverture manquante
```
