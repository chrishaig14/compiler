cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Winline -O0 -fverbose-asm")
add_library(
        core
        core/core.cpp
        core/core.h
        core/functions.h
        core/GC.cpp
        core/GC.h
        core/xobject.cpp
        core/xobject.h
        core/xobjects/XList.cpp
        core/xobjects/XList.cpp
        core/xobjects/XList.h
        core/xobjects/XString.cpp
        core/xobjects/XString.h
        core/xobjects/XObject.cpp
        core/xobjects/XObject.cpp
        core/xobjects/XObject.h
        core/xobjects/XDict.cpp
        core/xobjects/XDict.h
        core/xobjects/XFile.cpp
        core/xobjects/XFile.cpp
        core/xobjects/XFile.h
        core/xobjects/XTuple.cpp
        core/xobjects/XTuple.h macros.h core/basics.h core/xobjects/XDouble.cpp core/xobjects/XDouble.h core/basics.cpp)
