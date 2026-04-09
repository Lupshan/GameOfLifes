add_executable(engine_tests
    test_main.cpp
    test_version.cpp
    test_position.cpp
    test_direction.cpp
    test_grid.cpp
    test_rng.cpp
    test_world_config.cpp
    test_world.cpp
    test_simulation.cpp
    test_snapshot.cpp
    test_genome.cpp
    test_reproduction.cpp
    test_ecosystem.cpp
)
target_link_libraries(engine_tests PRIVATE engine_core doctest::doctest)

add_test(NAME engine_tests COMMAND engine_tests)
