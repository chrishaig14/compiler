cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
add_library(core core/core.cpp core/core.h IntegerObject.cpp StringObject.cpp ListObject.cpp Object.cpp NoneObject.cpp TupleObject.cpp UserObject.cpp BooleanObject.cpp core/functions.h core/GC.cpp core/GC.h core/xobject.cpp core/xobject.h core/xobjects/XList.cpp core/xobjects/XList.cpp core/xobjects/XList.h core/xobjects/XString.cpp core/xobjects/XString.h core/xobjects/XObject.cpp core/xobjects/XObject.cpp core/xobjects/XObject.h core/xobjects/XDict.cpp core/xobjects/XDict.h core/xobjects/XFile.cpp core/xobjects/XFile.cpp core/xobjects/XFile.h core/xobjects/Tuple.cpp core/xobjects/Tuple.h)
