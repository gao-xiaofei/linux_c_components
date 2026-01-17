cmake_minimum_required(VERSION 3.17)

project(doso_mqtt C)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
set(CMAKE_C_STANDARD 99)

add_executable(doso_mqtt main_mqtt.c)
target_link_libraries(doso_mqtt paho-mqtt3c)