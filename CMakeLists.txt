cmake_minimum_required(VERSION 3.5)
project(avlc)

#set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "--std=gnu++1z ${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
        src/main.cpp
        include/CompilerExceptions.h
        include/CompilerOptions.h
        include/State.h
        include/Scanner.h
        include/Token.h
        include/types.h
        include/Keywords.h
        include/packageHandler.h
        src/packageHandler.cpp
        src/Parser.cpp
        src/Scanner.cpp
        include/Parser.h include/Nodes.h
        src/standardPackage.cpp
        include/iir_utils.h
        src/iir_utils.cpp
        include/iir.h)


set( CMAKE_CXX_FLAGS "-fstack-protector -fstack-protector-all -Wall -Wextra -pedantic -Werror" )
set( CMAKE_CXX_FLAGS_DEBUG "-ggdb" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 " )

include_directories(libs)
include_directories(include)
include_directories(external/include)

add_executable(avlc ${SOURCE_FILES})