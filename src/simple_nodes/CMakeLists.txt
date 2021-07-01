cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")

add_library(
        simple_nodes
#        nodes/AliasNode.cpp
#        nodes/AliasNode.h
#        nodes/EnumNode.cpp
#        nodes/EnumNode.h
#        nodes/MatchExpressionNode.cpp
#        nodes/MatchExpressionNode.h
#        nodes/ThrowNode.cpp
#        nodes/ThrowNode.h
#        nodes/TryCatchNode.cpp
#        nodes/TryCatchNode.h
#        nodes/UnaryOpNode.cpp
#        nodes/UnaryOpNode.h
        AssignmentSNode.cpp
        AssignmentSNode.h
        BlockSNode.cpp
        BlockSNode.h
        BoolSNode.cpp
        BoolSNode.h
        BreakSNode.cpp
        BreakSNode.h
        CallSNode.cpp
        CallSNode.h
        ClassSNode.cpp
        ClassSNode.h
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
        ListSNode.cpp
        ListSNode.h
        MatchSNode.cpp
        MatchSNode.h
        NewObjectSNode.cpp
        NewObjectSNode.h
        NoneSNode.cpp
        NoneSNode.h
        ObjectMemberSNode.cpp
        ObjectMemberSNode.h
        ReturnSNode.cpp
        ReturnSNode.h
        SNode.cpp
        SNode.h
        StringSNode.cpp
        StringSNode.h
        TernarySNode.cpp
        TernarySNode.h
        ThrowSNode.cpp
        ThrowSNode.h
        TryCatchSNode.cpp
        TryCatchSNode.h
        WhileSNode.cpp
)
