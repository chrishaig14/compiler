cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 11)

add_library(srcLib lib.cpp Scanner.cpp Scanner.h Token.cpp Token.h color_codes.h Parser.cpp Parser.h nodes/Visitor.cpp nodes/Visitor.h nodes/ReturnNode.cpp nodes/ReturnNode.h nodes/ClassNode.cpp nodes/ClassNode.h nodes/StringNode.cpp nodes/StringNode.h nodes/BinopNode.cpp nodes/BinopNode.h nodes/Node.cpp nodes/Node.h nodes/MemberNode.cpp nodes/MemberNode.h nodes/ListNode.cpp nodes/ListNode.h nodes/SubscriptNode.cpp nodes/SubscriptNode.h nodes/nodes.h nodes/CallNode.cpp nodes/CallNode.h nodes/FunctionNode.cpp nodes/FunctionNode.h nodes/DeclarationNode.cpp nodes/DeclarationNode.h nodes/TypeNode.cpp nodes/TypeNode.h nodes/IfNode.cpp nodes/IfNode.h nodes/IdNode.cpp nodes/IdNode.h nodes/NumberNode.cpp nodes/NumberNode.h nodes/AssignmentNode.cpp nodes/AssignmentNode.h)