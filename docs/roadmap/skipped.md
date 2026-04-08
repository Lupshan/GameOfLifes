# Skipped / partial roadmap items

Questions ouvertes rencontrées en mode autonome. Le user les traite en lot. Ne pas traiter sans validation explicite.

## Format

```
### NN — slug — date
**Question** : ...
**Contexte** : ...
**Options envisagées** : ...
**Action prise** : skip / partial / workaround
```

---

### 04 — world-grid — 2026-04-09
**Question** : la roadmap step 04 demande un `Grid<T>` **toroïdal**, mais ADR 005 + 006 stipulent un monde **infini chunké** (32×32 par chunk), non toroïdal. Faut-il :
- (a) garder `Grid<T>` toroïdal comme primitive générique réutilisable pour un chunk (le wrap reste un helper utile dans certains contextes), et brancher le world au-dessus en mode chunké non-toroïdal ?
- (b) renommer / supprimer la sémantique toroïdale et faire un `Grid<T>` strict bornes-vérifiées ?

**Contexte** : la divergence est probablement un reliquat de pré-ADR 005. Les primitives `Position`/`Direction` sont saines dans les deux cas.

**Action prise** : option (a) — `Grid<T>` implémenté avec wrap toroïdal comme la roadmap le demande, pour ne pas bloquer step 04. Le world-level (step 06+) devra utiliser des chunks non-wrap par-dessus. À reconfirmer par le PM avant step 06.
