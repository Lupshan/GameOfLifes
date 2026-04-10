#!/usr/bin/env bash
# Database backup script for GameOfLifes.
# Run nightly via cron or docker compose exec.
#
# Credentials: use a .pgpass file (chmod 600) or PGPASSWORD env var.
# Never pass passwords as command-line arguments (visible in ps).
set -euo pipefail

BACKUP_DIR="${BACKUP_DIR:-/data/backups}"
PGHOST="${PGHOST:-localhost}"
PGPORT="${PGPORT:-5432}"
PGUSER="${PGUSER:-gol}"
PGDATABASE="${PGDATABASE:-gameoflifes}"
KEEP_DAYS="${KEEP_DAYS:-7}"

mkdir -p "$BACKUP_DIR"

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
FILENAME="$BACKUP_DIR/gameoflifes_$TIMESTAMP.sql.gz"

echo "[backup] Starting backup → $FILENAME"
pg_dump -h "$PGHOST" -p "$PGPORT" -U "$PGUSER" "$PGDATABASE" | gzip > "$FILENAME"
echo "[backup] Done ($(du -h "$FILENAME" | cut -f1))"

# Prune old backups.
find "$BACKUP_DIR" -name "gameoflifes_*.sql.gz" -mtime +"$KEEP_DAYS" -delete
echo "[backup] Pruned backups older than $KEEP_DAYS days"
