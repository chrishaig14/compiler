cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
add_subdirectory(src)
add_subdirectory(test)
add_executable(untitled main.cpp)
target_link_libraries(untitled srcLib)
