# 03 — Engine skeleton (CMake + doctest)

**Status**: not started
**Started**: —
**Finished**: —

## Goal
Stand up the C++ engine project with a working build, a working test runner, and a "hello tick" executable. No simulation logic yet — the point is that `just build-engine` and `just test-engine` work end-to-end and CI is green on a trivial test.

## Scope
- `engine/CMakeLists.txt` producing two targets:
  - `engine_core` (static lib, currently with one trivial source file).
  - `gameoflifes_engine` (executable linking `engine_core`, prints "tick 0" and exits).
- `doctest` fetched via `FetchContent`, pinned tag.
- `engine/tests/CMakeLists.txt` registering one trivial passing test (`CHECK(1 + 1 == 2)`).
- Compiler flags: `-std=c++23 -Wall -Wextra -Wpedantic -Werror`.
- `just build-engine`, `just test-engine`, `just run` recipes wired.

## Non-goals
- No `World`, no `Agent`, no real simulation (step 06).
- No external libs beyond doctest.
- No install targets.

## Technical notes
- Use `set(CMAKE_CXX_STANDARD 23)` + `set(CMAKE_CXX_STANDARD_REQUIRED ON)`.
- Build dir: `engine/build/` (gitignored).
- Namespace `gol::` declared in a placeholder `engine/include/engine/version.hpp`.
- Doctest tag: latest stable (`v2.4.11` at time of writing — verify before pinning).

## Definition of Done
- [ ] `cmake -S engine -B engine/build && cmake --build engine/build` succeeds with no warnings.
- [ ] `engine/build/gameoflifes_engine` runs and prints "tick 0".
- [ ] `ctest --test-dir engine/build` passes.
- [ ] `just build-engine`, `just test-engine`, `just run` all work.
- [ ] CI engine job green.
- [ ] `docs/roadmap/README.md` table updated to `done`.
