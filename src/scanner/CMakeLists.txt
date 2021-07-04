cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -pedantic -Werror")

add_library(
    scanner
    CodeLines.cpp
    CodeLines.h
    Scanner.cpp
    Scanner.h
    TextPosition.cpp
    TextPosition.h
    Token.cpp
    Token.h
    ../utils.cpp
    ../utils.h
)
