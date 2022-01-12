cmake_minimum_required(VERSION 3.6)

include(check_header.cmake)
include(check_function.cmake)

# check header
check_header("stdatomic.h")
check_header("sys/types.h")
check_header("sys/stat.h")
check_header("sys/time.h")
check_header("fcntl.h")
#check_header("stdint.h") #c99
#check_header("stdbool.h") #c99
#check_header("pthread.h") #posix

# check function
check_function("gettid" "unistd.h")
check_function("strlcpy" "string.h")
check_function("strlcat" "string.h")
check_function("clock_gettime" "time.h")
check_function("gettimeofday" "sys/time.h")
check_function("pthread_spin_lock" "pthread.h")
check_function("pthread_mutex_timedlock" "pthread.h")
check_function("sem_timedwait" "semaphore.h")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.tpl ${CMAKE_SOURCE_DIR}/include/event/e_config.h)
