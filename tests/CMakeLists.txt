cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -pedantic -Wno-return-type")
add_executable(parser_tests parser_tests.cpp catch.hpp)
target_link_libraries(parser_tests parser)