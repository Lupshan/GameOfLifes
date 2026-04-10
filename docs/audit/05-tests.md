# Partie 5 — Tests manquants

**Priorité** : P2 — couverture insuffisante sur des composants critiques.

## État actuel

| Brique | Tests | Couverture |
|--------|-------|------------|
| Engine C++ | ~120 assertions (doctest) | Bonne sur world/sim/VM, faible sur IPC/broadcast/lineage |
| Bytecode compiler | 62 tests (pytest) | Bonne |
| Backend | 40 tests (pytest) | Moyenne — pas de test IPC réel, pas de test rate limit |
| Frontend | 13 tests (vitest) | **Faible** — decode + smoke + playback + auth concept |

## Tâches

### Engine C++ (manquants)

- [ ] **IPC mode** — `engine/src/ipc.cpp`
  - Pas de test automatisé. Seul un smoke test manuel (`echo '{"cmd":"shutdown"}' | ./engine --ipc`)
  - Fix : écrire `test_ipc.cpp` qui lance un subprocess, envoie des commandes, vérifie les events

- [ ] **Lineage log** — `engine/src/lineage_log.cpp`
  - Aucun test. Le log est utilisé en production mais jamais vérifié
  - Fix : écrire `test_lineage_log.cpp` — log birth/death, relire le fichier, parser le JSONL

- [ ] **Broadcast** — `engine/tests/test_broadcast.cpp`
  - Existe mais minimal (4 tests)
  - Manque : broadcast avec radius > grille, broadcast toroïdal, listen sans broadcast

- [ ] **Stress test 1000+ agents**
  - Aucun test de charge
  - Fix : test fonctionnel qui spawne 1000 agents, tourne 100 ticks, vérifie pas de crash et invariants maintenus

### Backend Python (manquants)

- [ ] **Engine bridge** — `backend/app/engine/process.py`
  - `test_engine_bridge.py` ne teste que le protocole (sérialisation), pas le subprocess réel
  - Fix : test d'intégration avec mock subprocess ou subprocess réel si engine est build

- [ ] **Rate limiting**
  - Aucun test vérifiant que le rate limit fonctionne
  - Fix : test envoyant N+1 requêtes et vérifiant le 429

- [ ] **Snapshot pruning**
  - `prune_snapshots()` est testé unitairement mais jamais appelé. Pas de test d'intégration
  - Fix : test créant des fichiers datés, appelant prune, vérifiant la suppression

- [ ] **WebSocket multi-client race**
  - Tests WS existants sont séquentiels. Pas de test de concurrence
  - Fix : test avec 10 clients se connectant/déconnectant en parallèle

### Frontend Svelte (manquants)

- [ ] **Blockly generator** — `frontend/src/lib/blocks/generator.ts`
  - Aucun test. C'est le composant le plus critique côté frontend (block → source)
  - Fix : tests unitaires pour chaque type de bloc (move, if, while, let, perceive_food)

- [ ] **API fetch** — `frontend/src/lib/api.ts`
  - Test auth.test.ts vérifie le concept mais pas `apiFetch()` directement
  - Fix : mock fetch, vérifier Bearer header, vérifier erreur handling

- [ ] **Canvas renderer** — `frontend/src/lib/canvas/renderer.ts`
  - Impossible à tester directement (Canvas2D API), mais les fonctions de décodage sont testées
  - Fix : au minimum tester `agentColor()` et les fonctions d'extraction de données
