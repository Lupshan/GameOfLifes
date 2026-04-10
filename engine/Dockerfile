# Multi-stage build for the C++ simulation engine.
FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    clang cmake ninja-build git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY engine/ ./

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++ \
    && cmake --build build

# Runtime stage.
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd -g 1001 gol && useradd -u 1001 -g gol -m gol

COPY --from=build /src/build/gameoflifes_engine /usr/local/bin/gameoflifes_engine
COPY engine/config/ /etc/gameoflifes/

USER gol
ENTRYPOINT ["gameoflifes_engine"]
CMD ["--ipc", "--config", "/etc/gameoflifes/default.toml"]
