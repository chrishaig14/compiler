cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")

add_library(
        scanner
        Scanner.cpp
        Scanner.h
        Token.cpp
        Token.h
        CodeLines.cpp
        CodeLines.h
        TextPosition.cpp
        TextPosition.h)