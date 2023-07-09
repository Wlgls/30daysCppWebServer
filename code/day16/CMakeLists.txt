
cmake_minimum_required(VERSION 3.10)
project(WebServer)


include_directories(
    base
    tcp
)

SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

find_package(Threads REQUIRED)

aux_source_directory(base SRC_LIST1)
aux_source_directory(tcp SRC_LIST2)


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/test/)

add_executable(echoserver test/echo_server.cpp ${SRC_LIST1} ${SRC_LIST2} )
target_link_libraries(echoserver ${CMAKE_THREAD_LIBS_INIT})

