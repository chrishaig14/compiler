cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(spdlog)
add_executable(compiler src/compiler.cpp)
target_link_libraries(compiler scanner nodes parser srcLib)
add_executable(runner src/runner.cpp)


# ---------------------------------------------------------------------------------------
# Example of using pre-compiled library
# ---------------------------------------------------------------------------------------
#set(spdlog_DIR spdlog/build)
#if(NOT TARGET spdlog)
#    # Stand-alone build
#endif()
target_link_libraries(runner scanner translator parser srcLib nodes  spdlog::spdlog)
