# Partie 4 — Frontend (Svelte)

**Priorité** : P2 — robustesse et hygiène.

## Tâches

- [x] **C11. WS reconnexion sans backoff exponentiel**
  - Fichier : `frontend/src/routes/world/+page.svelte`
  - Fix : backoff exponentiel avec jitter implémenté

- [x] **C12. Pas de validation des snapshots WS**
  - Fichier : `frontend/src/routes/world/+page.svelte`
  - Fix : fonction `isValidSnapshot()` ajoutée

- [ ] **M2. Frontend — couverture de tests très faible**
  - Manquent :
    - [ ] Tests du generator Blockly (block → source GoL Script)
    - [ ] Tests de `api.ts` (token injection, erreur handling)
    - [ ] Tests unitaires du renderer (mock canvas)

- [x] **M5. Types API dupliqués dans chaque page**
  - Fix : centralisés dans `frontend/src/lib/types/api.ts`

- [x] **M6. NO_PARENT hardcodé en magic number**
  - Fichier : `frontend/src/lib/components/AgentInspector.svelte`
  - Fix : constante `NO_PARENT` extraite

- [x] **M7. Pas de check Prettier dans la CI**
  - Fichier : `.github/workflows/ci.yml`
  - Fix : ajouté

- [x] **M9. Pas d'ADR pour les choix frontend**
  - Fix : `docs/adr/018-frontend-stack.md` créé
