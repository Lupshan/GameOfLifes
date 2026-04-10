# Partie 5 — Tests manquants

**Priorité** : P2 — couverture insuffisante sur des composants critiques.

## État actuel

| Brique | Tests | Couverture |
|--------|-------|------------|
| Engine C++ | ~120 assertions (doctest) | Bonne sur world/sim/VM/broadcast/lineage/stress |
| Bytecode compiler | 62 tests (pytest) | Bonne |
| Backend | 40+ tests (pytest) | Bonne — rate limit testé |
| Frontend | 17 tests (vitest) | Moyenne — decode + smoke + playback + auth + snapshot |

## Tâches

### Engine C++ (manquants)

- [ ] **IPC mode** — `engine/src/ipc.cpp`
  - Pas de test automatisé. Seul un smoke test manuel
  - Fix : écrire `test_ipc.cpp` qui lance un subprocess

- [x] **Lineage log** — `engine/src/lineage_log.cpp`
  - Fix : `test_lineage_log.cpp` ajouté

- [x] **Broadcast** — `engine/tests/test_broadcast.cpp`
  - Fix : `test_broadcast_extended.cpp` avec tests toroïdaux et edge cases

- [x] **Stress test 1000+ agents**
  - Fix : `test_stress.cpp` ajouté

### Backend Python (manquants)

- [ ] **Engine bridge** — `backend/app/engine/process.py`
  - Pas de test d'intégration avec subprocess réel

- [x] **Rate limiting**
  - Fix : `test_rate_limit.py` ajouté

- [ ] **Snapshot pruning**
  - Manque un test d'intégration end-to-end

- [ ] **WebSocket multi-client race**
  - Pas de test de concurrence multi-client

### Frontend Svelte (manquants)

- [ ] **Blockly generator** — `frontend/src/lib/blocks/generator.ts`
  - Aucun test. Composant critique

- [ ] **API fetch** — `frontend/src/lib/api.ts`
  - Pas de test direct de `apiFetch()`

- [ ] **Canvas renderer** — `frontend/src/lib/canvas/renderer.ts`
  - Fonctions de décodage testées, mais pas les fonctions de rendu
