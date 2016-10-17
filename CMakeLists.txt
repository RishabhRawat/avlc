cmake_minimum_required(VERSION 3.5)
project(avlc)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp
        include/CompilerExceptions.h
        include/CompilerOptions.h
        src/Parser.cpp
        include/Parser.h)


set( CMAKE_CXX_FLAGS "-fstack-protector -fstack-protector-all" )
set( CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall" )

include_directories(libs)
include_directories(include)

add_executable(avlc ${SOURCE_FILES})