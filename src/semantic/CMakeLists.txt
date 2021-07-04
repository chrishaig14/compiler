cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0 -Wall -pedantic -Werror")

add_library(
    semantic
    ../macros.h
    ../types.h
    ../utils.cpp
    ../utils.h
    CheckCall.cpp
    CheckCall.h
    CheckDeclaration.cpp
    CheckDeclaration.h
    Checker.cpp
    Checker.h
    CheckerUnify.cpp
    CheckerUtils.cpp
    CheckerUtils.h
    CheckerVisit.cpp
    CheckExpressions.cpp
    CheckExpressions.h
    CheckLiterals.cpp
    CheckLiterals.h
    CheckMember.cpp
    CheckMember.h
    CheckStatements.cpp
    CheckStatements.h
    ErrorReporter.cpp
    ErrorReporter.h
    GlobalProcessor.cpp
    SemanticInfo.cpp
    SemanticInfo.h
    SymbolTable.cpp
    SymbolTable.h
    util.cpp
    util.h)

target_link_libraries(semantic units simple_nodes)
