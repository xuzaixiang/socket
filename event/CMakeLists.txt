cmake_minimum_required(VERSION 3.6)



add_library(event
        src/e_event.c
        src/e_loop.c
        )

target_include_directories(event PRIVATE include)