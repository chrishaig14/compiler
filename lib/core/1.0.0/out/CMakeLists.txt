cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Winline -O0 -fverbose-asm")
add_library(
        core
        core.cpp
        core.h
        Function.h
        GC.cpp
        GC.h
        xobject.cpp
        xobject.h
        xobjects/XList.cpp
        xobjects/XList.cpp
        xobjects/XList.h
        xobjects/XString.cpp
        xobjects/XString.h
        xobjects/XObject.cpp
        xobjects/XObject.cpp
        xobjects/XObject.h
        xobjects/XDict.cpp
        xobjects/XDict.h
        xobjects/XFile.cpp
        xobjects/XFile.cpp
        xobjects/XFile.h
        xobjects/XTuple.cpp
        xobjects/XTuple.h macros.h basics.h xobjects/XDouble.cpp xobjects/XDouble.h basics.cpp Function.cpp xobjects/Integer.cpp xobjects/Integer.h xobjects/Boolean.cpp xobjects/Boolean.h xobjects/Float.cpp xobjects/Float.h functions.cpp functions.h)
