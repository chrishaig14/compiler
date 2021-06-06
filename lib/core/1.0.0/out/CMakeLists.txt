cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Winline -O0 -fverbose-asm")
add_library(
        core-1.0.0
        core/core.cpp
        core/core.h
        core/Function.h
        core/GC.cpp
        core/GC.h
        core/threads.cpp
        core/threads.h
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
        core/xobjects/XSocket.cpp
        core/xobjects/XSocket.h
        core/xobjects/XTuple.cpp
        core/xobjects/XTuple.h core/macros.h core/basics.h core/xobjects/XDouble.cpp core/xobjects/XDouble.h core/basics.cpp core/Function.cpp core/xobjects/Integer.cpp core/xobjects/Integer.h core/xobjects/Boolean.cpp core/xobjects/Boolean.h core/xobjects/Float.cpp core/xobjects/Float.h core/functions.cpp core/functions.h)
