cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

add_library(
        simple_nodes
        with_unique/Assignment.cpp
        with_unique/Assignment.h
        Block.cpp
        Block.h
        with_unique/Bool.cpp
        with_unique/Bool.h
        with_unique/Break.cpp
        with_unique/Break.h
        with_unique/Call.cpp
        with_unique/Call.h
        KlassDef.cpp
        KlassDef.h
        with_unique/Continue.cpp
        with_unique/Continue.h
        Declaration.cpp
        Declaration.h
        with_unique/Dict.cpp
        with_unique/Dict.h
        with_unique/EnumMember.cpp
        with_unique/EnumMember.h
        with_unique/EnumDef.cpp
        with_unique/EnumDef.h
        with_unique/Float.cpp
        with_unique/Float.h
        with_unique/FunctionDef.cpp
        with_unique/FunctionDef.cpp
        with_unique/FunctionDef.h
        with_unique/FunctionDef.h
        with_unique/Id.cpp
        with_unique/Id.h
        with_unique/IfSNode.cpp
        with_unique/IfSNode.h
        with_unique/Integer.cpp
        with_unique/Integer.h
        with_unique/List.cpp
        with_unique/List.h
        Match.cpp
        Match.h
        NewObject.cpp
        NewObject.h
        with_unique/None.cpp
        with_unique/None.h
        with_unique/ObjectMember.cpp
        with_unique/ObjectMember.h
        with_unique/Return.cpp
        with_unique/Return.h
        SNode.cpp
        SNode.h
        with_unique/String.cpp
        with_unique/String.h
        Ternary.cpp
        Ternary.h
        Throw.cpp
        Throw.h
        TryCatch.cpp
        TryCatch.h
        with_unique/While.cpp sem.h with_unique/ObjectMethodCall.cpp with_unique/ObjectMethodCall.h with_unique/ObjectMethod.cpp with_unique/ObjectMethod.h with_unique/ConstFunctionCall.cpp with_unique/ConstFunctionCall.h with_unique/ConstFunction.cpp with_unique/ConstFunction.h)
