# Stage 1: Build the C++ simulation engine.
FROM ubuntu:24.04 AS engine-build

RUN apt-get update && apt-get install -y --no-install-recommends \
    clang cmake ninja-build git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY engine/ ./

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++ \
    && cmake --build build

# Stage 2: Python backend with embedded engine binary.
FROM python:3.12-slim

# libstdc++6 needed by the C++ engine binary.
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Install uv.
COPY --from=ghcr.io/astral-sh/uv:latest /uv /usr/local/bin/uv

WORKDIR /app

# Copy bytecode package (path dependency).
COPY bytecode/ /bytecode/

# Copy backend.
COPY backend/pyproject.toml backend/uv.lock ./
RUN uv sync --frozen --no-dev

COPY backend/app/ ./app/
COPY backend/alembic.ini ./
COPY backend/alembic/ ./alembic/

# Copy engine binary and config from build stage.
COPY --from=engine-build /src/build/gameoflifes_engine /usr/local/bin/gameoflifes_engine
COPY engine/config/ /etc/gameoflifes/

RUN groupadd -g 1001 gol && useradd -u 1001 -g gol -m gol \
    && chown -R gol:gol /app
USER gol

EXPOSE 8000

CMD ["uv", "run", "--frozen", "uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
