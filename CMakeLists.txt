cmake_minimum_required(VERSION 3.6)
project(event VERSION 0.0.1)

#include(CTest)# add BUILD_TESTING option
#include(CMakeDependentOption)
#
#cmake_dependent_option(EVENT_BUILD_TESTS
#        "Build the unit tests when BUILD_TESTING is enabled and we are the root project" ON
#        "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
#cmake_dependent_option(EVENT_BUILD_BENCH
#        "Build the benchmarks when building unit tests and we are the root project" ON
#        "EVENT_BUILD_TESTS" OFF)

#if (WIN32)
#else ()
#
#endif ()
#
#if (EVENT_BUILD_TESTS)
#    list(APPEND event_test_sources test/run.c)
#    list(APPEND event_test_sources test/runner.c)
#    add_executable(event_run_tests ${event_test_sources})
#else ()
#    message("-------")
#endif ()

add_subdirectory(examples)
add_subdirectory(source)
