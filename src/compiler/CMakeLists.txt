cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")
find_package(fmt)

add_library(
        lcomp
        analyze.cpp
        analyze.h
        Compiler.cpp
        Compiler.h
        parse.cpp
        parse.h
        utils.cpp
        utils.h)
add_executable(compiler main.cpp)
target_link_libraries(
        compiler
        python
        lcomp
        parser
        semantic
        scanner
        units
        nodes
        fmt::fmt
)
