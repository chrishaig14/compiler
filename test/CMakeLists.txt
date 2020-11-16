cmake_minimum_required(VERSION 3.16)
project(untitled_test)
include_directories(../src)
set(CMAKE_CXX_STANDARD 11)

add_executable(scanner_test
        scanner_test.cpp)
target_link_libraries(scanner_test gtest gtest_main pthread srcLib)

add_executable(checker_test
        checker_test.cpp node_equals.cpp)
target_link_libraries(checker_test gtest gtest_main pthread srcLib)

add_executable(parser_test
        parser_test.cpp)
target_link_libraries(parser_test gtest gtest_main pthread srcLib)

add_executable(env_test
        env_test.cpp)
target_link_libraries(env_test gtest gtest_main pthread srcLib)

add_executable(node_equals
        node_equals.cpp)
target_link_libraries(node_equals gtest gtest_main pthread srcLib)

add_executable(translator_test
        translator_test.cpp)
target_link_libraries(translator_test gtest gtest_main pthread srcLib)

add_executable(test_all
        scanner_test.cpp parser_test.cpp checker_test.cpp)
target_link_libraries(test_all gtest gtest_main pthread srcLib)