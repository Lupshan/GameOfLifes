# Partie 1 — Sécurité

**Priorité** : P0 — à faire avant tout déploiement public.

## Tâches

- [ ] **B5. JWT secret par défaut en prod**
  - Fichier : `backend/app/config.py`
  - Problème : `jwt_secret = "change-me-in-production"` sans validation
  - Fix : fail au startup si `jwt_secret` == valeur par défaut et `debug=false`
  - Ajouter un `@field_validator` sur Settings

- [ ] **B7. Token JWT stocké dans localStorage**
  - Fichier : `frontend/src/lib/stores/auth.ts`, `frontend/src/lib/api.ts`
  - Problème : vulnérable au XSS — tout script injecté peut lire le token
  - Fix : migrer vers cookie httpOnly
    - Backend : `Set-Cookie` httpOnly/Secure/SameSite=Lax sur login/signup
    - Frontend : supprimer localStorage, le cookie est envoyé automatiquement
    - `backend/app/auth/router.py` doit retourner le cookie au lieu du token dans le body

- [ ] **B8. Docker — containers en root**
  - Fichiers : `engine/Dockerfile`, `backend/Dockerfile`, `frontend/Dockerfile`
  - Fix : ajouter dans chaque Dockerfile :
    ```dockerfile
    RUN addgroup -g 1000 gol && adduser -D -u 1000 -G gol gol
    USER gol
    ```

- [ ] **B9. Docker — pas de health checks**
  - Fichier : `docker-compose.yml`
  - Fix : ajouter `healthcheck` sur backend, frontend, engine
    ```yaml
    backend:
      healthcheck:
        test: ["CMD", "curl", "-f", "http://localhost:8000/health"]
        interval: 10s
        timeout: 5s
        retries: 3
    ```

- [ ] **C14. Nginx — headers de sécurité manquants**
  - Fichier : `frontend/nginx.conf`
  - Fix : ajouter CSP, X-Content-Type-Options, X-Frame-Options, Referrer-Policy
    ```nginx
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-Frame-Options "DENY" always;
    add_header Content-Security-Policy "default-src 'self'; script-src 'self' 'unsafe-inline'; style-src 'self' 'unsafe-inline';" always;
    ```

- [ ] **M8. Mot de passe Postgres par défaut**
  - Fichier : `docker-compose.yml`
  - Fix : remplacer `${POSTGRES_PASSWORD:-changeme}` par `${POSTGRES_PASSWORD:?POSTGRES_PASSWORD required}`

- [ ] **M12. Backup script expose le mot de passe**
  - Fichier : `scripts/backup.sh`
  - Fix : utiliser `.pgpass` au lieu de passer l'URL en argument
