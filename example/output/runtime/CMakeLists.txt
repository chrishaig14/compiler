cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
add_library(core core/core.cpp core/core.h IntegerObject.cpp StringObject.cpp ListObject.cpp Object.cpp NoneObject.cpp TupleObject.cpp UserObject.cpp BooleanObject.cpp core/functions.h core/GC.cpp core/GC.h core/xobject.cpp core/xobject.h)
