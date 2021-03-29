cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -O3 -fverbose-asm -Winline")
add_subdirectory(runtime)
add_executable(result lib.cpp project.cpp another.cpp)
target_link_libraries(result core)
