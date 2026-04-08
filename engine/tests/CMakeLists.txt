add_executable(engine_tests
    test_version.cpp
)
target_link_libraries(engine_tests PRIVATE engine_core doctest::doctest)
target_compile_definitions(engine_tests PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

add_test(NAME engine_tests COMMAND engine_tests)
