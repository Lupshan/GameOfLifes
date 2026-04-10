# Deployment Guide

## Quick start (local)

```bash
docker compose up --build
```

This starts: PostgreSQL, engine (IPC mode), backend (FastAPI), frontend (nginx), Prometheus.

- Frontend: http://localhost
- Backend API: http://localhost:8000
- Prometheus: http://localhost:9090

## Environment variables

| Variable | Default | Description |
|----------|---------|-------------|
| `POSTGRES_PASSWORD` | `changeme` | PostgreSQL password |
| `JWT_SECRET` | `change-me-in-production` | JWT signing secret |
| `GOL_DATABASE_URL` | (set in compose) | Async database URL |
| `GOL_DEBUG` | `false` | Enable debug logging |

## Production

1. Set strong secrets via `.env` file (not committed).
2. Use a reverse proxy (nginx/caddy) with TLS in front of the compose stack.
3. Consider separate Postgres instance (managed DB).
4. Back up the database regularly (see `scripts/backup.sh` in step 37).

## Dev mode

```bash
docker compose -f docker-compose.yml -f docker-compose.dev.yml up --build
```

Backend code is bind-mounted for hot reload via uvicorn `--reload`.

## CI/CD

The release workflow (`.github/workflows/release.yml`) builds and pushes Docker images on git tags matching `v*`.
