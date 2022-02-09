cmake_minimum_required(VERSION 3.6)

option(BUILD_SHARED "build shared library" ON)


add_subdirectory(config)
set(SRC src/loop/e_loop_new.c
        src/loop/e_loop_free.c
        src/loop/e_loop_run.c
        src/io/e_io_free.c
        src/e_mutex.c
        src/e_event.c
        src/e_alloc.c
        src/e_thread.c
        src/e_iowatcher.h
        )

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND SRC src/linux/e_sys_epoll.c)
endif ()

if (BUILD_SHARED)
    add_library(event
            SHARED
            ${SRC}
            )
    target_compile_definitions(event PRIVATE EVENT_DYNAMIC)
else ()
    add_library(event
            ${SRC}
            )
endif ()

target_include_directories(event PRIVATE include)


if (EVENT_BUILD_EXAMPLES)
    message("--------- EVENT_BUILD_EXAMPLES --")
    add_subdirectory(example)
endif ()