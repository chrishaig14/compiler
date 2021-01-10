cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_subdirectory(src)
add_subdirectory(example/build)
add_subdirectory(test)
add_executable(runner src/runner.cpp)
find_package(fmt)
add_executable(compiler src/full.cpp)

target_link_libraries(
  compiler
  translator
  parser
  srcLib
  nodes
  scanner
  transpiler
  tinyxml
  fmt::fmt)
