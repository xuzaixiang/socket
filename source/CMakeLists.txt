cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(config)
add_library(event test.c)

#target_link_libraries(event)
target_include_directories(event PUBLIC config)