cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 11)

add_library(srcLib lib.cpp Scanner.cpp Scanner.h Token.cpp Token.h color_codes.h Parser.cpp Parser.h AstNode.cpp AstNode.h replacer.cpp replacer.h)