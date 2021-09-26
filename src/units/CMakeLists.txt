cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")
add_library(
        units
        Class.cpp
        Class.h
        ConstFunction.cpp
        ConstFunction.h
        entities/Entity.cpp
        entities/Entity.h
        Enum.cpp
        Enum.h
        FunctionValue.cpp
        FunctionValue.h
        Module.cpp
        Module.h
        ObjectValue.cpp
        ObjectValue.h
        Package.cpp
        Package.h
        ../util/macros.h
        ../util/types.h modulemember/ModuleMember.cpp modulemember/ModuleMember.h entities/EntityClass.cpp entities/EntityClass.h entities/EntityModule.cpp entities/EntityModule.h entities/EntityConstFunction.cpp entities/EntityConstFunction.h entities/EntityEnum.cpp entities/EntityEnum.h entities/EntityNotFound.cpp entities/EntityNotFound.h entities/EntityError.cpp entities/EntityError.h entities/EntityNothing.cpp entities/EntityNothing.h entities/EntityPackage.cpp entities/EntityPackage.h entities/EntityValue.cpp entities/EntityValue.h)
