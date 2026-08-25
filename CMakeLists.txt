cmake_minimum_required(VERSION 3.10)
project(struct2json
    VERSION 2.0.4
    LANGUAGES C
    DESCRIPTION "C struct to JSON serialization library"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# ---- struct2json library (static) ----

add_library(struct2json STATIC
    struct2json/src/s2j.c
    cJSON/cJSON.c
)
target_include_directories(struct2json
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/struct2json/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
)
target_link_libraries(struct2json PUBLIC m)

# ---- Demo executable ----

add_executable(demo
    demo/main.c
    demo/my_struct_2_json.c
)
target_include_directories(demo PRIVATE demo/inc)
target_link_libraries(demo PRIVATE struct2json)
target_compile_definitions(demo PRIVATE DEBUGS2J)
target_compile_options(demo PRIVATE -Wno-int-conversion)

# ---- Tests (only when building standalone or explicitly enabled) ----

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    option(BUILD_TESTING "Build test suite" ON)
else()
    option(BUILD_TESTING "Build test suite" OFF)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
