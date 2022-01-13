cmake_minimum_required(VERSION 3.6)
project(event C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(example)

add_library(event
        SHARED
        source/e_loop.c
        )
target_include_directories(event PRIVATE include)