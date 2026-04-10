# Partie 1 — Sécurité

**Priorité** : P0 — à faire avant tout déploiement public.

## Tâches

- [x] **B5. JWT secret par défaut en prod**
  - Fichier : `backend/app/config.py`
  - Fix : fail au startup si `jwt_secret` == valeur par défaut et `debug=false`

- [x] **B7. Token JWT stocké dans localStorage**
  - Fichiers : `frontend/src/lib/stores/auth.ts`, `frontend/src/lib/api.ts`
  - Fix : migré vers cookie httpOnly (Secure, SameSite=Lax)

- [x] **B8. Docker — containers en root**
  - Fichiers : `engine/Dockerfile`, `backend/Dockerfile`, `frontend/Dockerfile`
  - Fix : non-root user `gol:1000` dans tous les Dockerfiles

- [x] **B9. Docker — pas de health checks**
  - Fichier : `docker-compose.yml`
  - Fix : healthcheck ajouté sur backend, frontend, postgres

- [x] **C14. Nginx — headers de sécurité manquants**
  - Fichier : `frontend/nginx.conf`
  - Fix : CSP, X-Content-Type-Options, X-Frame-Options ajoutés

- [x] **M8. Mot de passe Postgres par défaut**
  - Fichier : `docker-compose.yml`
  - Fix : `${POSTGRES_PASSWORD:?POSTGRES_PASSWORD required}`

- [x] **M12. Backup script expose le mot de passe**
  - Fichier : `scripts/backup.sh`
  - Fix : utilise `.pgpass` au lieu de passer le mdp en argument
