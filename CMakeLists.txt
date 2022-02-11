cmake_minimum_required(VERSION 3.6)
project(socket)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CMakeDependentOption)
add_subdirectory(event)

cmake_dependent_option(EVENT_BUILD_EXAMPLES
        "Build examples and we are the root project" ON
        "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

#include(define.cmake)
#option(OPTION_EVENT_ALLOC_COUNT "count alloc !" ON)
#option(OPTION_EVENT_ALLOC_PRINT "whether pint alloc info or not !" ON)
#
#option_define(${OPTION_EVENT_ALLOC_COUNT} "EVENT_ALLOC_COUNT")
#option_define(${OPTION_EVENT_ALLOC_PRINT} "EVENT_ALLOC_PRINT")



#if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
#    add_subdirectory(example)
#endif ()
#
#set(event_source
#        source/e_loop.c
#        source/e_io.c
#        source/e_nio.c
#        source/e_event.c
#        source/e_socket.c
#        source/e_sockaddr.c
#        source/loop/e_server.c
#        source/io/e_fields.c
#        source/util/e_alloc.c
#        source/util/e_string.c
#        source/util/e_math.c
#        )
#
#if (CMAKE_SYSTEM_NAME MATCHES "Linux")
#    list(APPEND event_source source/linux/e_sys_epoll.c)
#elseif (APPLE)
#    list(APPEND event_source source/mac/e_kqueue.c)
#endif ()
#
#add_library(event
#        SHARED
#        ${event_source}
#        )
#target_include_directories(event PRIVATE include)