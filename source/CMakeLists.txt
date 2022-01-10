cmake_minimum_required(VERSION 3.6)

macro(with_option option)
#    string(SUBSTRING ${option} 7 -1 out)
#    add_definitions(-D${out})
    add_definitions(-D${option})
endmacro()

option(OPTION_EVENT_ALLOC_COUNT   "count alloc !"                       ON)
option(OPTION_EVENT_ALLOC_PRINT   "whether pint alloc info or not !"    ON)

with_option(OPTION_EVENT_ALLOC_COUNT)
with_option(OPTION_EVENT_ALLOC_PRINT)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

#if ()
#
#endif ()

add_subdirectory(config)

add_definitions(-DEVENT_ALLOC_COUNT)
add_definitions(-DEVENT_ALLOC_PRINT)

add_subdirectory(../third_party/cus/source cus)
add_library(event
        test.c
        eevent.c
        epoll.c
        eloop.c
        ealloc.c
        )

target_include_directories(event PUBLIC ../include)
target_include_directories(event PRIVATE ../third_party/cus/include)
target_include_directories(event PUBLIC config)