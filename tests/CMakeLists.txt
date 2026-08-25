cmake_minimum_required(VERSION 3.10)

# Test executable
add_executable(test_s2j
    test_runner.c
)
target_include_directories(test_s2j PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_s2j PRIVATE struct2json)

# Register with CTest
add_test(NAME struct2json_tests COMMAND test_s2j)

# Convenience: "make check" builds and runs tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_s2j
)
