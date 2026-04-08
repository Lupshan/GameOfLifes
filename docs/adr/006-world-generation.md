# ADR 006 — World generation (infinite, biome-coherent, lazy)

**Status**: accepted
**Date**: 2026-04-08

## Context

L'ADR 005 fige le principe d'un monde infini généré paresseusement à mesure de l'exploration, avec des biomes spatialement cohérents (pas de lac perdu en plein désert). Cet ADR fige le *comment*.

## Decision

### Chunks

- Le monde est découpé en **chunks** carrés de taille fixe (valeur initiale : **32 × 32 cases**).
- Un chunk est **généré la première fois qu'une case qu'il contient est observée ou visitée** par un agent (ou réclamée par un client en lecture, pour le viewer).
- Une fois généré, un chunk est **persisté** (snapshot DB) et ne sera plus régénéré : son état évolue ensuite via la simulation.
- Les chunks froids (aucun agent dedans, aucune observation récente) peuvent être déchargés de la mémoire et rechargés à la demande.

### Génération déterministe

- **Seed du monde** unique (entier 64 bits), figé à la création du monde partagé.
- La génération d'un chunk dépend uniquement de `(world_seed, chunk_x, chunk_y)` → reproductible, testable, et permet de régénérer un chunk depuis zéro si la DB est perdue (tant qu'aucune simulation n'a encore tourné dessus).
- RNG : voir ADR existant `05-rng-config` (pas de `std::rand`).

### Biomes

Liste v1 (validée avec le PM) :

1. **Plaine** — neutre, nourriture moyenne, régénération moyenne.
2. **Forêt** — nourriture abondante, vision réduite.
3. **Forêt tropicale** — nourriture très abondante, humide, vision très réduite, propice aux maladies.
4. **Désert** — nourriture rare, régénération lente, température extrême (mort sans gène adapté).
5. **Plage** — transition côtière, nourriture faible mais accessible, traversable par tous.
6. **Lac / rivière** — eau douce. Traversable par tout agent, mais dégâts continus et déplacement très coûteux sans gène aquatique ou vol.
7. **Océan** — vaste, faune propre. Traversable par tout agent, mais létal rapidement sans gène aquatique ou vol.
8. **Marécage** — déplacement coûteux (coût en ticks accru), nourriture moyenne, propice aux maladies.
9. **Collines rocheuses** — semi-difficile, nourriture faible, abrite des proies adaptées.
10. **Montagne** — déplacement très coûteux pour les marcheurs, peu de nourriture, température basse (dégâts sans adaptation). Triviale pour les volants.

> Rappel cohérent avec l'ADR 005 : **aucun biome n'est physiquement infranchissable**. Tout terrain est traversable, l'environnement hostile se traduit par dégâts continus + ralentissement, pas par un mur.

(Inspiration générale : régions d'Amérique latine — gradient plage → forêt tropicale → collines → montagne très représenté.)

### Cohérence spatiale

Approche à base de **bruit 2D continu** sur deux axes climatiques :

- `temperature(x, y)` — bruit Simplex/OpenSimplex à basse fréquence, plage `[-1, 1]` (froid → chaud).
- `humidity(x, y)` — bruit Simplex/OpenSimplex à basse fréquence indépendant, plage `[-1, 1]` (sec → humide).

Le biome d'une case est déterminé par sa position dans le plan **(température × humidité)** via une **table de classification** (whittaker-like). Exemples de cellules de la table :

- froid + sec → montagne / collines rocheuses
- froid + humide → forêt
- chaud + sec → désert
- chaud + humide → forêt tropicale
- modéré + intermédiaire → plaine
- niveau d'eau (3ᵉ bruit, basse fréquence) → océan / lac / plage selon la profondeur, override les autres biomes.

La basse fréquence des bruits garantit la cohérence : les biomes s'étendent sur plusieurs chunks, les transitions sont graduelles, **pas de case isolée incohérente**.

### Contenu d'un chunk fraîchement généré

À la génération, pour chaque case du chunk :

- biome (déterminé par les bruits ci-dessus),
- état initial de nourriture (selon le biome),
- entités initiales éventuelles (faune passive, plantes), tirées d'une table par biome avec un RNG seedé sur `(world_seed, chunk_x, chunk_y, "spawn")`.

Aucun **agent de bot user** n'est jamais spawné par la génération de chunk : les agents user n'apparaissent que via la procédure de spawn de bot (ADR 005).

## Consequences

- Le moteur doit gérer un cache LRU de chunks chargés et un mécanisme de chargement/déchargement transparent.
- La table de classification biome est un point d'équilibrage central : elle vivra dans `engine/include/engine/world/biome_table.hpp` (à créer le moment venu) et sera modifiable sans toucher au code de génération.
- Le déterminisme `(seed, x, y) → chunk` permet d'écrire des **tests fonctionnels** sur la génération (snapshot d'un chunk attendu pour un seed connu).
- Le format de bruit choisi (Simplex / OpenSimplex) entraînera l'ajout d'une petite dépendance ou d'une implémentation maison ; à trancher dans le step d'implémentation correspondant, en respectant la dependency policy (CLAUDE.md).
- Le viewer frontend (Polytopia-style semi-3D) devra demander les chunks au backend par coordonnées et gérer le streaming au scroll. Pas de contrainte côté ADR de génération, mais à garder en tête pour l'API WS.
