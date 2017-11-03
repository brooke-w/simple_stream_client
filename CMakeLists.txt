cmake_minimum_required(VERSION 3.8)
project(simple_stream_client)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(simple_stream_client ${SOURCE_FILES})