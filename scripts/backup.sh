#!/usr/bin/env bash
# Database backup script for GameOfLifes.
# Run nightly via cron or docker compose exec.
set -euo pipefail

BACKUP_DIR="${BACKUP_DIR:-/data/backups}"
DB_URL="${GOL_DATABASE_URL:-postgresql://gol:changeme@localhost:5432/gameoflifes}"
KEEP_DAYS="${KEEP_DAYS:-7}"

mkdir -p "$BACKUP_DIR"

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
FILENAME="$BACKUP_DIR/gameoflifes_$TIMESTAMP.sql.gz"

echo "[backup] Starting backup → $FILENAME"
pg_dump "$DB_URL" | gzip > "$FILENAME"
echo "[backup] Done ($(du -h "$FILENAME" | cut -f1))"

# Prune old backups.
find "$BACKUP_DIR" -name "gameoflifes_*.sql.gz" -mtime +"$KEEP_DAYS" -delete
echo "[backup] Pruned backups older than $KEEP_DAYS days"
