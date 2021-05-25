cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_CLANG_TIDY clang-tidy-12 --checks=-*,readability-function-cognitive-complexity)
#set(CMAKE_CXX_CPPCHECK  cppcheck --enable=all --suppress=missingIncludeSystem)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -Werror -pedantic -Wno-return-type")
add_subdirectory(src)
find_package(fmt)
add_executable(compiler src/compiler/full.cpp src/compiler/utils.cpp src/compiler/utils.h src/compiler/transpile.cpp src/compiler/transpile.h src/compiler/analyze.cpp src/compiler/analyze.h src/compiler/parse.cpp src/compiler/parse.h lib/core/1.0.0/out/runtime.h)
target_link_libraries(
        compiler
        parser
        srcLib
        nodes
        scanner
        transpiler
        fmt::fmt)
