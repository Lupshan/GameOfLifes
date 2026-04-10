cmake_minimum_required(VERSION 3.20)
project(gameoflifes_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --- engine_core static library ---

add_library(engine_core STATIC
    src/version.cpp
    src/direction.cpp
    src/rng.cpp
    src/world_config_loader.cpp
    src/world.cpp
    src/simulation.cpp
    src/stats.cpp
    src/snapshot_json.cpp
    src/genome.cpp
    src/traits.cpp
    src/lineage_log.cpp
    src/terrain.cpp
    src/world_clock.cpp
    src/bytecode.cpp
    src/vm.cpp
    src/agent_vm.cpp
    src/ipc.cpp
    src/ml_runtime.cpp
    src/brain_bytecode.cpp
    src/brain_ml.cpp
    src/reserve.cpp
    src/brain_learning.cpp
)
target_include_directories(engine_core PUBLIC include)
target_link_libraries(engine_core PUBLIC tomlplusplus::tomlplusplus nlohmann_json::nlohmann_json)

# --- gameoflifes_engine executable ---

add_executable(gameoflifes_engine
    src/main.cpp
)
target_link_libraries(gameoflifes_engine PRIVATE engine_core)

# --- tests ---

enable_testing()
add_subdirectory(tests)
