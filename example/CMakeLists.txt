cmake_minimum_required(VERSION 3.6)
project(event C)

set(CMAKE_C_STANDARD 11)

include_directories(../include)

add_executable(test main.c)

add_executable(example_tcp_echo_server tcp_echo_server.c)
target_link_libraries(example_tcp_echo_server event)