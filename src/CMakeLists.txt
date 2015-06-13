cmake_minimum_required(VERSION 3.2)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(src ${SOURCE_FILES} praser.cpp praser.h sender.cpp sender.h server.cpp server.h header_file.h)