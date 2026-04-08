# 36 — Deployment

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Package the app so it runs the same way locally and in production. Docker images for each brick, a compose file for local + e2e, and a documented prod deployment recipe.

## Scope
- `engine/Dockerfile` — multi-stage, builds the C++ binary, slim runtime image.
- `backend/Dockerfile` — Python slim base, uv install, app code.
- `frontend/Dockerfile` — node build, nginx-serve static.
- `docker-compose.yml` at repo root: postgres + backend + engine + frontend + prometheus.
- `docker-compose.dev.yml` override for local dev with bind mounts.
- Prod deployment doc `docs/deployment.md`: env vars, secrets, reverse proxy hints, backup strategy.
- GH Actions release workflow building + pushing images on tag.

## Non-goals
- No Kubernetes manifests (compose is enough for v1).
- No managed cloud deploy script.

## Technical notes
- Engine image must include the runtime libs of any deps (zstd, onnxruntime).
- All secrets via env, never baked into images.

## Definition of Done
- [ ] All Dockerfiles build.
- [ ] `docker compose up` brings the full stack online locally.
- [ ] e2e test from step 30 runs against the composed stack.
- [ ] Release workflow works on a test tag.
- [ ] `docs/roadmap/README.md` table updated to `done`.
