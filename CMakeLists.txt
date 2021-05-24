cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_CLANG_TIDY clang-tidy -extra-arg=-Wno-unknown-warning-option --checks=-*,readability-function-cognitive-complexity)
#set(CMAKE_CXX_CPPCHECK  cppcheck --enable=all --suppress=missingIncludeSystem)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -Werror -pedantic -Wno-return-type")
add_subdirectory(src)
add_subdirectory(test)
find_package(fmt)
add_executable(compiler src/compiler/full.cpp)
target_link_libraries(
        compiler
        parser
        srcLib
        nodes
        scanner
        transpiler
        tinyxml
        fmt::fmt)
