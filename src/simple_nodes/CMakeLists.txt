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
        Klass.cpp
        Klass.h
        ContinueSNode.cpp
        ContinueSNode.h
        DeclarationSNode.cpp
        DeclarationSNode.h
        DictSNode.cpp
        DictSNode.h
        EnumMemberSNode.cpp
        EnumMemberSNode.h
        EnumSNode.cpp
        EnumSNode.h
        FloatSNode.cpp
        FloatSNode.h
        FunctionSNode.cpp
        FunctionSNode.cpp
        FunctionSNode.h
        FunctionSNode.h
        IdSNode.cpp
        IdSNode.h
        IfSNode.cpp
        IfSNode.h
        IntegerSNode.cpp
        IntegerSNode.h
        List.cpp
        List.h
        Match.cpp
        Match.h
        NewObjectSNode.cpp
        NewObjectSNode.h
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
        While.cpp sem.h)
