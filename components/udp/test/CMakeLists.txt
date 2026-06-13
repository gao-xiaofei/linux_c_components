cmake_minimum_required(VERSION 3.16)

project(doso_udp_test 
    LANGUAGES C
    VERSION 1.0.0
    DESCRIPTION "udp test Project"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_SOURCES 
        "./main_udp.c"
        "../src/*.c"
)

add_executable(doso_udp_test ${SRC_SOURCES})

target_include_directories(doso_udp_test 
    PRIVATE 
        /usr/local/include 
        ../src
)

target_link_directories(doso_udp_test 
    PRIVATE 
        /usr/local/lib
)

target_link_libraries(doso_udp_test
    PRIVATE
        pthread
        m
)

# 编译选项
target_compile_options(doso_udp_test PRIVATE -Wall -Wextra -Werror -O2)