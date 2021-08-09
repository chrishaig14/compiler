cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

#add_executable(nodes_tests nodes_test.cpp catch.hpp)
#target_link_libraries(nodes_tests nodes scanner)
#
#add_executable(parser_tests parser_tests.cpp catch.hpp)
#target_link_libraries(parser_tests parser)

add_executable(all_tests parser_tests.cpp nodes_test.cpp global_tests.cpp all_tests.cpp checker_tests.cpp catch.hpp)
target_link_libraries(all_tests lcomp nodes scanner parser semantic transpiler)