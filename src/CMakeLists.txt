cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 11)

add_library(srcLib lib.cpp Scanner.cpp Scanner.h Token.cpp Token.h color_codes.h Parser.cpp Parser.h AstNode.cpp AstNode.h FirstPass.cpp FirstPass.h SecondPass.cpp SecondPass.h SymbolTable.cpp SymbolTable.h ClassTable.cpp ClassTable.h VirtualMachine.cpp VirtualMachine.h Frame.cpp Frame.h Inst.cpp Inst.h Object.cpp Object.h ObjectStack.h ObjectStack.cpp Environment.cpp Environment.h)