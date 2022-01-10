cmake_minimum_required(VERSION 3.6)
project(event C)

add_executable(test_eloop test_eloop.c)
target_link_libraries(test_eloop event)