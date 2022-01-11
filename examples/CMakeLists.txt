cmake_minimum_required(VERSION 3.6)
project(event C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)


add_executable(test_eloop test_eloop.c)
target_link_libraries(test_eloop event)

add_executable(example_tcp_echo_server example_tcp_echo_server.c)
target_link_libraries(example_tcp_echo_server event)

add_executable(test test.c)