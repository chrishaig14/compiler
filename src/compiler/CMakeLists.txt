cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -pedantic -Wno-return-type")
find_package(fmt)

add_executable(
    compiler
    full.cpp
    utils.cpp
    utils.h
    transpile.cpp
    transpile.h
    analyze.cpp
    analyze.h
    parse.cpp
    parse.h
    Compiler.cpp
    Compiler.h)

target_link_libraries(
    compiler
    parser
    semantic
    scanner
    transpiler
    units
    nodes
    fmt::fmt)
