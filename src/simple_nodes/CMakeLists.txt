cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

add_library(
        simple_nodes
        Assignment.cpp
        Assignment.h
        Block.cpp
        Block.h
        Bool.cpp
        Bool.h
        Break.cpp
        Break.h
        Call.cpp
        Call.h
        KlassDef.cpp
        KlassDef.h
        Continue.cpp
        Continue.h
        Declaration.cpp
        Declaration.h
        Dict.cpp
        Dict.h
        EnumMember.cpp
        EnumMember.h
        EnumDef.cpp
        EnumDef.h
        Float.cpp
        Float.h
        FunctionDef.cpp
        FunctionDef.cpp
        FunctionDef.h
        FunctionDef.h
        Id.cpp
        Id.h
        IfSNode.cpp
        IfSNode.h
        Integer.cpp
        Integer.h
        List.cpp
        List.h
        Match.cpp
        Match.h
        NewObject.cpp
        NewObject.h
        None.cpp
        None.h
        ObjectMember.cpp
        ObjectMember.h
        Return.cpp
        Return.h
        SNode.cpp
        SNode.h
        String.cpp
        String.h
        Ternary.cpp
        Ternary.h
        Throw.cpp
        Throw.h
        TryCatch.cpp
        TryCatch.h
        While.cpp sem.h ObjectMethodCall.cpp ObjectMethodCall.h ObjectMethod.cpp ObjectMethod.h ConstFunctionCall.cpp ConstFunctionCall.h ConstFunction.cpp ConstFunction.h)
