cmake_minimum_required(VERSION 3.17)

project(doso_cjson_test 
    LANGUAGES C
    VERSION 1.0.0
    DESCRIPTION "Eclipse Paho MQTT C Client Project"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_SOURCES 
        "./main_cjson.c"
        "../src/*.c"
)

add_executable(doso_cjson_test ${SRC_SOURCES})

target_include_directories(doso_cjson_test 
    PRIVATE 
        /usr/local/include 
        ../src
)

target_link_directories(doso_cjson_test 
    PRIVATE 
        /usr/local/lib
)

target_link_libraries(doso_cjson_test
    PRIVATE
        pthread
        m
)

# 编译选项
target_compile_options(doso_cjson_test PRIVATE -Wall -Wextra -Werror -O2)