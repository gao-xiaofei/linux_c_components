cmake_minimum_required(VERSION 3.17)

project(doso_mqtt 
    LANGUAGES C
    VERSION 1.0.0
    DESCRIPTION "Eclipse Paho MQTT C Client Project"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_SOURCES 
        "./main_mqtt.c"
)

add_executable(doso_mqtt ${SRC_SOURCES})

target_include_directories(doso_mqtt 
    PRIVATE 
        /usr/local/include 
)

target_link_directories(doso_mqtt 
    PRIVATE 
        /usr/local/lib
)

target_link_libraries(doso_mqtt
    PRIVATE
        paho-mqtt3c
        pthread
)

# 编译选项
target_compile_options(doso_mqtt PRIVATE -Wall -Wextra -Werror -O2)