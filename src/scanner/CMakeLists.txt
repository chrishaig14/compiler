cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

add_library(scanner Scanner.cpp Scanner.h)

target_link_libraries(scanner common util)
