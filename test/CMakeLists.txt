cmake_minimum_required(VERSION 3.16)
project(untitled_test)
include_directories(../src)
set(CMAKE_CXX_STANDARD 11)

add_executable(runTests scanner_test.cpp parser_test.cpp replacer_test.cpp utils.cpp utils.h first_pass_test.cpp second_pass_test.cpp)
target_link_libraries(runTests gtest gtest_main pthread srcLib)