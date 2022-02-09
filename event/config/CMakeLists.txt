cmake_minimum_required(VERSION 3.6)

include(check_header.cmake)
include(check_function.cmake)

check_function("gettid" "unistd.h")
check_function("eventfd" "sys/eventfd.h")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/e_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/../src/e_config.h)

