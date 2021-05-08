cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_subdirectory(src)
add_subdirectory(runtime)
add_subdirectory(test)
find_package(fmt)
add_executable(compiler src/full.cpp)
target_link_libraries(
        compiler
        parser
        srcLib
        nodes
        scanner
        transpiler
        tinyxml
        fmt::fmt)
add_subdirectory(bootstrap/output)