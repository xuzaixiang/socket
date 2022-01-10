cmake_minimum_required(VERSION 3.6)
project(cus C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(cus
#        cus_alloc.c
        cus_str.c
        )