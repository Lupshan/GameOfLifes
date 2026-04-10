# Partie 4 — Frontend (Svelte)

**Priorité** : P2 — robustesse et hygiène.

## Tâches

- [ ] **C11. WS reconnexion sans backoff exponentiel**
  - Fichier : `frontend/src/routes/world/+page.svelte` (ligne ~36)
  - Problème : retry fixe 2s → thundering herd si serveur down
  - Fix : backoff exponentiel avec jitter
    ```ts
    let retryDelay = 1000;
    ws.onclose = () => {
        setTimeout(connectWS, retryDelay + Math.random() * 500);
        retryDelay = Math.min(retryDelay * 2, 30000);
    };
    // Reset retryDelay on successful connection
    ws.onopen = () => { retryDelay = 1000; };
    ```

- [ ] **C12. Pas de validation des snapshots WS**
  - Fichier : `frontend/src/routes/world/+page.svelte` (ligne ~28)
  - Problème : `JSON.parse(event.data) as Snapshot` sans vérification
  - Fix : créer une fonction `isValidSnapshot(obj): obj is Snapshot` avec checks sur les champs requis

- [ ] **M2. Frontend — couverture de tests très faible**
  - 13 tests : smoke, decode (5), playback (5), auth store (2)
  - Manquent :
    - [ ] Tests du generator Blockly (block → source GoL Script)
    - [ ] Tests de `api.ts` (token injection, erreur handling)
    - [ ] Tests unitaires du renderer (mock canvas)
  - Au minimum ajouter les tests du generator

- [ ] **M5. Types API dupliqués dans chaque page**
  - Fichiers : `routes/stats/+page.svelte`, `routes/genealogy/[bot_id]/+page.svelte`, `routes/me/bots/+page.svelte`, etc.
  - Chaque page redéfinit `interface Bot`, `interface LineageEvent`, etc.
  - Fix : créer `frontend/src/lib/types/api.ts` avec toutes les interfaces et importer partout

- [ ] **M6. NO_PARENT hardcodé en magic number**
  - Fichier : `frontend/src/lib/components/AgentInspector.svelte` (ligne ~24)
  - Problème : `agent.parent_id === 18446744073709551615`
  - Fix : `const NO_PARENT = 18446744073709551615n;` dans `types/api.ts` — ou mieux, faire retourner `null` par le backend

- [ ] **M7. Pas de check Prettier dans la CI**
  - Fichier : `.github/workflows/ci.yml`
  - Fix : ajouter dans le job `frontend` :
    ```yaml
    - run: cd frontend && npx prettier --check .
    ```

- [ ] **M9. Pas d'ADR pour les choix frontend**
  - Fix : écrire `docs/adr/018-frontend-stack.md` documentant :
    - Pourquoi SvelteKit 2 + Svelte 5
    - Pourquoi Canvas2D (pas WebGL)
    - Pourquoi localStorage pour les tokens (temporaire, migrer vers cookies)
