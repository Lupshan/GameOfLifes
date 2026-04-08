# ADR 005 — Game rules v1

**Status**: accepted
**Date**: 2026-04-08

## Context

Validation des règles de base du jeu avec le PM. Le projet n'a pas de winner / loser : l'objectif est d'observer un écosystème vivant. Cet ADR fige les règles minimales pour la v1 — assez pour implémenter, assez peu pour rester ajustable par playtest.

## Decision

### Monde

- **Grille carrée** type échiquier, **toroïdale ? non — infinie** par génération paresseuse à base de chunks (voir ADR 006).
- **Un seul monde partagé persistant**, qui tourne 24/7 côté serveur. Les bots vivent leur vie en permanence, indépendamment de la présence des users connectés. Mondes privés / shards : pas en v1.

### Tick

- **1 tick = 100 ms** (10 ticks/s) en v1, ajustable.
- Les actions des agents **coûtent des ticks** (modèle Zappy). Coûts fixes en v1, pas de budget custom par bot — on garde l'idée "budget réparti par le user" pour plus tard.
- Coûts indicatifs (à équilibrer par playtest) :
  - `avancer` : 7 ticks
  - `tourner` (gauche / droite, 90°) : 7 ticks
  - `voir` : 7 ticks
  - `manger` : 7 ticks
  - `se reproduire` : ~50 ticks
  - actions sociales / combat : à définir au moment de leur implémentation

### Agents

- **Orientation 4 directions** (N / E / S / O).
- **Vision** :
  - **Perception passive de la case courante** : un agent connaît toujours, gratuitement et à chaque tick, le contenu de la case sur laquelle il se trouve (biome, nourriture présente, autres agents présents).
  - **Action `voir`** (coût en ticks) : révèle la **case directement devant** l'agent. C'est la vision de base, accessible à tous.
  - **Vision périphérique** : une mutation génétique (non garantie, apparaît par évolution) peut étendre `voir` aux **2 cases en diagonale avant** en plus de la case devant. Sans cette mutation, les diagonales avant restent invisibles.
- **Collisions** : plusieurs agents peuvent occuper la même case. Au-delà d'un seuil de population locale (valeur initiale : 5), malus de métabolisme et risque accru de transmission de maladie. Pas de cap dur.
- **Gènes vs comportement** : séparation stricte.
  - Les **gènes** sont hérités + mutés à la reproduction. Ils définissent les *capacités physiques* de l'agent : métabolisme, régime alimentaire (gradient herbivore↔carnivore), vitesse, portée de vision, agressivité, mode de déplacement (marche / vol), résistance par biome, espérance de vie max. Le bot ne les écrit pas.
  - Le **bytecode du bot** définit le *comportement* : quoi faire avec ces capacités. Le bot peut lire ses propres gènes mais pas les modifier directement.
- **Mort permanente.** Causes possibles en v1 : faim, vieillesse (âge max génétique), blessures de combat, température extrême (biome incompatible), maladie, environnement hostile (eau pour un non-aquatique, montagne pour un non-volant, etc.).
- **Tout terrain est traversable**, mais à un coût. Aucun biome n'est *physiquement* infranchissable : un agent terrestre *peut* entrer dans un lac, simplement il y subit des dégâts continus (et meurt vite sans gène aquatique). Idem montagne pour un non-volant. Le coût de déplacement (en ticks) varie aussi selon le biome — certains biomes ralentissent fortement les agents non adaptés.
- **Vol** : mutation génétique. Un agent volant ignore les pénalités de déplacement liées au terrain (toutes les cases lui coûtent le même nombre de ticks de base), se déplace plus vite que les marcheurs, et ne subit aucun dégât d'environnement hostile sur les biomes qu'il survole. En contrepartie : coût énergétique accru (métabolisme plus rapide → faim plus vite). Le vol modifie aussi la dynamique prédateur/proie : un volant échappe facilement à un terrestre, ou inversement plonge sur lui pour l'attaquer.

### Bots

- **Lecture stricte de l'isolation inter-bots** (cf. discussion PM) : aucun canal de communication ni de perception "logique" entre agents issus de bots différents. Les agents d'un autre bot sont **visibles physiquement** (apparaissent dans `voir`) mais aucune primitive du langage ne permet de communiquer, échanger, ou coordonner avec eux. Cela évite la coopération inter-bots déguisée.
- **Reproduction inter-bots interdite** : un agent ne peut se reproduire qu'avec un agent du même bot. Pas d'hybrides en v1 (potentiellement v2).
- **Spawn d'un nouveau bot** : à la soumission, N agents (valeur initiale : 3) apparaissent dans une **zone aléatoire suffisamment éloignée** des bots existants pour éviter le spawn-kill. Distance minimale à calibrer.
- **Extinction** : si tous les agents d'un bot meurent, le bot rejoint la liste **"dead cells"** — un mémorial public, sans effet de jeu (pas de fork, pas de respawn auto). Le user peut soumettre une nouvelle version.

### Écosystème

- **Nourriture = double système** : entités physiques sur la grille (plantes qui poussent, proies qui se déplacent), **et** un taux de régénération propre à chaque biome qui pilote leur réapparition.
- **Prédation autorisée et encouragée** : un agent au régime suffisamment carnivore peut tuer et manger un autre agent (de n'importe quel bot). Les stratégies d'évitement (fuite, vol pour échapper aux marcheurs, camouflage par adaptation au biome) sont attendues comme conséquence émergente du système de gènes — pas de mécanique scriptée pour ça.

## Consequences

- L'isolation stricte simplifie le design du langage et du VM (pas de primitives de communication inter-bots à concevoir).
- Le double système de nourriture ajoute un peu de complexité au moteur mais permet une vraie pression écologique.
- Les coûts de ticks indicatifs sont des points de départ : ils seront ajustés par playtest et chaque ajustement notable mérite une note dans le roadmap step concerné.
- La persistance 24/7 impose au moteur de tourner en service long et au backend de gérer les snapshots réguliers (voir ADR existant snapshot serialization).
- Beaucoup de paramètres numériques (seuils, coûts, distances) sont volontairement laissés "à calibrer" : ils seront figés progressivement dans `engine/include/engine/config.hpp` au fur et à mesure de l'implémentation.
