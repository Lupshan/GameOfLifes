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

# --- engine_core static library ---

add_library(engine_core STATIC
    src/version.cpp
)
target_include_directories(engine_core PUBLIC include)

# --- gameoflifes_engine executable ---

add_executable(gameoflifes_engine
    src/main.cpp
)
target_link_libraries(gameoflifes_engine PRIVATE engine_core)

# --- tests ---

enable_testing()
add_subdirectory(tests)
