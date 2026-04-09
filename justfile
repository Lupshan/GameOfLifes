# GameOfLifes — unified task runner
# Usage: `just <recipe>`. Run `just` with no args to list recipes.

default:
    @just --list

# --- build ---

# Configure and build the C++ engine (Debug by default)
build-engine:
    @if [ -f engine/CMakeLists.txt ]; then \
        cmake -S engine -B engine/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
        cmake --build engine/build -j ; \
    else \
        echo "engine/CMakeLists.txt not present yet — skipping build (see roadmap step 03)." ; \
    fi

# Build everything that needs building
build: build-engine

# --- test ---

# Run engine tests
test-engine: build-engine
    @if [ -d engine/build ]; then \
        ctest --test-dir engine/build --output-on-failure ; \
    else \
        echo "No engine build — skipping engine tests." ; \
    fi

# Run bytecode compiler tests
test-bytecode:
    cd bytecode && uv run pytest

# Run backend tests
test-backend:
    cd backend && uv run pytest

# Run all tests
test: test-engine test-bytecode test-backend

# --- run ---

# Start the simulation engine
run: build-engine
    ./engine/build/gameoflifes_engine

# --- compile ---

# Compile a GoL Script source file to bytecode
compile file:
    cd bytecode && uv run python -m compiler ../{{file}}

# --- format & lint ---

# Format all code
fmt:
    @cpp_files=$(find engine/include engine/src engine/tests -type f \( -name '*.cpp' -o -name '*.hpp' \) 2>/dev/null) ; \
    if [ -n "$cpp_files" ]; then echo "$cpp_files" | xargs clang-format -i ; \
    else echo "No C++ sources to format yet." ; fi
    cd bytecode && uv run ruff format .
    cd backend && uv run ruff format .

# Lint all code
lint:
    cd bytecode && uv run ruff check .
    cd backend && uv run ruff check .

# Type-check Python code (pyright basic mode)
typecheck:
    cd bytecode && uv run pyright
    cd backend && uv run pyright

# --- housekeeping ---

# Wipe all build artifacts
clean:
    rm -rf engine/build
    find . -type d -name __pycache__ -exec rm -rf {} +
    find . -type d -name .pytest_cache -exec rm -rf {} +
