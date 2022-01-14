cmake_minimum_required(VERSION 3.6)
project(event C)

set(CMAKE_C_STANDARD 11)

add_definitions(-DEVENT_ALLOC_COUNT)
add_definitions(-DEVENT_ALLOC_PRINT)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(example)
endif ()

set(event_source
        source/e_loop.c
        source/e_io.c
        source/util/e_alloc.c
        )

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND event_source source/linux/e_sys_epoll.c)
endif ()

add_library(event
        SHARED
        ${event_source}
        )
target_include_directories(event PRIVATE include)