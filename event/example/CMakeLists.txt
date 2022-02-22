cmake_minimum_required(VERSION 3.6)


add_executable(test_loop test_loop.c)
target_include_directories(test_loop PRIVATE ../include)
target_link_libraries(test_loop event pthread)
