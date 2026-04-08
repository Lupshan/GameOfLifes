# 35 — Observability

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Make the running system debuggable in production. Structured logs, metrics, and a minimum set of dashboards before exposing the app to the world.

## Scope
- Structured logging (JSON) in backend (using `structlog`) and engine (`spdlog` with JSON sink).
- Metrics: Prometheus exporter on backend (`/metrics`) — engine emits metrics over IPC, backend re-exposes.
- Key metrics: tick rate, agent count, snapshot lag, WS connection count, request latency, compile error rate.
- Health endpoint upgraded: checks DB, engine subprocess, recent tick activity.
- Tracing (optional, document deferred): OpenTelemetry hooks but not required for v1.

## Non-goals
- No log aggregation server (assume external — Loki / Grafana Cloud).
- No alerting rules.

## Technical notes
- Log fields normalized: `ts`, `level`, `event`, `bot_id`, `tick`, etc.
- Metrics names follow Prometheus conventions.

## Definition of Done
- [ ] Logs are structured everywhere.
- [ ] `/metrics` exposes the listed metrics.
- [ ] Manual scrape works locally with a Prometheus container.
- [ ] `docs/roadmap/README.md` table updated to `done`.
