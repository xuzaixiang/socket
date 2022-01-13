cmake_minimum_required(VERSION 3.6)
project(event C)

set(CMAKE_C_STANDARD 11)

add_executable(test main.c)

add_executable(example_tcp_echo_server tcp_echo_server.c)