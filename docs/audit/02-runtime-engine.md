# Partie 2 — Runtime engine (C++)

**Priorité** : P1 — bugs et undefined behavior dans le hot path.

## Tâches

- [ ] **B1. VM — Overflow entier silencieux dans ADD/SUB/MUL**
  - Fichier : `engine/src/vm.cpp` (lignes ~163, 173, 183)
  - Problème : `push(a + b)` wraps silencieusement en int32
  - Fix : au choix :
    - Documenter comme "wrapping arithmetic" (comme la plupart des VMs) et ajouter un test
    - Ou détecter l'overflow et retourner `VmStatus::Overflow`

- [ ] **B2. VM — Null pointer si step() avant load()**
  - Fichier : `engine/src/vm.cpp` (ligne ~89)
  - Problème : `bc_->code.size()` dereference nullptr
  - Fix : ajouter `if (bc_ == nullptr) return VmStatus::Halted;` en tête de `step()`

- [ ] **B3. VM — Sémantique JMP confuse**
  - Fichier : `engine/src/vm.cpp` (lignes ~284-295)
  - Problème : `target = pc - 2 + offset` non documenté
  - Fix :
    - Ajouter un commentaire clair : "offset relatif à la position du byte après l'opérande"
    - Ajouter des tests : JMP +0 (boucle infinie), JMP négatif, JMP au-delà du code

- [ ] **B4. Agent index stale dans AgentIntrinsicHandler**
  - Fichier : `engine/src/agent_vm.cpp` (méthode `agent()`)
  - Problème : `world_.agents()[agent_index_]` sans bounds check
  - Fix : ajouter `assert(agent_index_ < world_.agents().size())` ou bounds check avec erreur propre

- [ ] **C4. O(N) perceive pour compter les agents**
  - Fichier : `engine/src/agent_vm.cpp` (lignes 31-36)
  - Problème : boucle sur tous les agents pour compter ceux sur la case courante
  - Fix : maintenir un `Grid<int>` de comptage agents mis à jour en début de tick

- [ ] **C5. O(N) lookup agent par ID dans BytecodeBrain**
  - Fichier : `engine/src/brain_bytecode.cpp` (lignes 18-22)
  - Fix : passer l'index en paramètre de `tick()` ou maintenir un `std::unordered_map<uint64_t, size_t>`

- [ ] **C13. Snapshot version hardcodée à 2**
  - Fichier : `engine/include/engine/snapshot.hpp` (ligne 27)
  - Fix : ne pas hardcoder dans le struct, préserver la version lue du JSON

- [ ] **C15. Mutation incohérente entre brains**
  - Fichiers : `engine/src/brain_ml.cpp`, `engine/src/simulation.cpp`
  - Problème : `MlBrain::execute_action` reproduit sans mutate()
  - Fix : appeler `mutate()` dans le case 5 (reproduce) de `MlBrain`

- [ ] **M10. noexcept manquant sur fonctions pures**
  - Fichiers : `engine/src/direction.cpp`, `terrain.cpp`, `traits.cpp`, `world_clock.cpp`
  - Fix : ajouter `noexcept` sur `to_offset()`, `turn_left()`, `movement_cost()`, `day_phase()`, etc.

- [ ] **M11. Hydration initialisée à starting_energy**
  - Fichier : `engine/src/world.cpp` (ligne 82)
  - Fix : ajouter `starting_hydration` dans `WorldConfig` ou documenter le choix
