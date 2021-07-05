cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")
add_library(
    units
    Class.cpp
    Class.h
    ConstFunction.cpp
    ConstFunction.h
    Entity.cpp
    Entity.h
    Enum.cpp
    Enum.h
    FunctionValue.cpp
    FunctionValue.h
    Module.cpp
    Module.h
    ObjectValue.cpp
    ObjectValue.h
    Package.cpp
    Package.h
    ../macros.h
    ../types.h)
