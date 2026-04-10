# Partie 2 — Runtime engine (C++)

**Priorité** : P1 — bugs et undefined behavior dans le hot path.

## Tâches

- [x] **B1. VM — Overflow entier silencieux dans ADD/SUB/MUL**
  - Fichier : `engine/src/vm.cpp` (lignes ~163, 173, 183)
  - Fix : documenté comme "wrapping arithmetic" (intentionnel, comme la plupart des VMs)

- [x] **B2. VM — Null pointer si step() avant load()**
  - Fichier : `engine/src/vm.cpp` (ligne ~95)
  - Fix : `if (bc_ == nullptr)` check ajouté en tête de `step()`

- [x] **B3. VM — Sémantique JMP confuse**
  - Fichier : `engine/src/vm.cpp` (lignes ~288-295)
  - Fix : commentaire détaillé avec exemples ajouté

- [x] **B4. Agent index stale dans AgentIntrinsicHandler**
  - Fichier : `engine/src/agent_vm.cpp`
  - Fix : bounds check ajouté

- [x] **C4. O(N) perceive pour compter les agents**
  - Fichier : `engine/src/agent_vm.cpp`
  - Fix : `Grid<int> agent_counts_` maintenu via `rebuild_caches()`

- [x] **C5. O(N) lookup agent par ID dans BytecodeBrain**
  - Fichier : `engine/src/brain_bytecode.cpp`
  - Fix : `std::unordered_map<uint64_t, size_t> agent_index_map_`

- [x] **C13. Snapshot version hardcodée à 2**
  - Fichier : `engine/include/engine/snapshot.hpp`
  - Fix : `SNAPSHOT_FORMAT_VERSION` constante extraite

- [x] **C15. Mutation incohérente entre brains**
  - Fichiers : `engine/src/brain_ml.cpp`
  - Fix : `mutate()` est bien appelé dans le case 5 (reproduce) de `MlBrain`

- [x] **M10. noexcept manquant sur fonctions pures**
  - Fichiers : `engine/src/terrain.cpp`, `traits.cpp`, `world_clock.cpp` + headers
  - Fix : `noexcept` ajouté sur toutes les fonctions pures (`direction.cpp` l'avait déjà)

- [x] **M11. Hydration initialisée à starting_energy**
  - Fichier : `engine/src/world.cpp` (ligne 102)
  - Fix : `spawn_child` utilise maintenant `config_.starting_hydration`
