cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

#STranspiler.cpp STranspiler.h
add_library(transpiler PythonModuleTranspiler.cpp PythonModuleTranspiler.h)
