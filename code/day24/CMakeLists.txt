cmake_minimum_required(VERSION 3.10)
project(WebServer)


include_directories(
    base
    timer
    log
    tcp
    http
)

SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

find_package(Threads REQUIRED)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} )

aux_source_directory(base SRC_LIST1)
aux_source_directory(timer SRC_LIST2)
aux_source_directory(log SRC_LIST3)
aux_source_directory(tcp SRC_LIST4)
aux_source_directory(http SRC_LIST5)



set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/test/)
# add_executable(WebServer http_server.cpp ${SRC_LIST1}  ${SRC_LIST3} ${SRC_LIST4})
add_executable(WebServer http_server.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})
# add_executable(test_logstream test/test_logstream.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})
target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})