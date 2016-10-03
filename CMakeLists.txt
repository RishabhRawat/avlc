cmake_minimum_required(VERSION 3.5)
project(avlc)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
    src/main.cpp)

add_executable(avlc ${SOURCE_FILES})