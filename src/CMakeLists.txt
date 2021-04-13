cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_library(tinyxml tinyxml/tinyxml2.cpp tinyxml/tinyxml2.h)

add_library(
  scanner
  scanner/Scanner.cpp
  scanner/Scanner.h
  scanner/Token.cpp
  scanner/Token.h
  scanner/CodeLines.cpp
  scanner/CodeLines.h
  scanner/TextPosition.cpp
  scanner/TextPosition.h)

add_library(
        nodes
        nodes/ReturnNode.cpp
        nodes/ReturnNode.h
        nodes/StringNode.cpp
        nodes/StringNode.h
        nodes/BreakNode.cpp
        nodes/BreakNode.h
        nodes/BinopNode.cpp
        nodes/BinopNode.h
        nodes/Node.cpp
        nodes/Node.h
        nodes/MemberNode.cpp
        nodes/MemberNode.h
        nodes/ListNode.cpp
        nodes/ListNode.h
        nodes/SubscriptNode.cpp
        nodes/SubscriptNode.h
        nodes/nodes.h
        nodes/CallNode.cpp
        nodes/CallNode.h
        nodes/FunctionNode.cpp
        nodes/FunctionNode.h
        nodes/DeclarationNode.cpp
        nodes/DeclarationNode.h
        nodes/TypeNode.cpp
        nodes/TypeNode.h
        nodes/IfNode.cpp
        nodes/TernaryNode.cpp
        nodes/TernaryNode.h
        nodes/NoneNode.cpp
        nodes/NoneNode.h
        nodes/IfNode.h
        nodes/ImportNode.h
        nodes/ImportNode.cpp
        nodes/IdNode.cpp
        nodes/ClassNode.cpp
        nodes/ClassNode.h
        nodes/IdNode.h
        nodes/NumberNode.cpp
        nodes/NumberNode.h
        nodes/AssignmentNode.cpp
        nodes/AssignmentNode.h
        nodes/BlockNode.cpp
        nodes/BlockNode.h
        nodes/EmptyListNode.cpp
        nodes/EmptyListNode.h
        nodes/BooleanNode.cpp
        nodes/BooleanNode.h
        nodes/WhileNode.cpp
        nodes/WhileNode.h
        nodes/ContinueNode.cpp
        nodes/ContinueNode.h
        nodes/Node.cpp
        nodes/Node.h
        nodes/ForNode.cpp
        nodes/ForNode.h
        nodes/optypes.h
        nodes/optypes.cpp
        nodes/TupleNode.cpp
        nodes/TupleNode.h
        nodes/BoolOpNode.cpp
        nodes/BoolOpNode.h
        nodes/PartialApplication.cpp
        nodes/PartialApplication.h
        nodes/DictNode.cpp
        nodes/DictNode.h
        nodes/EmptyDictNode.cpp
        nodes/EmptyDictNode.h nodes/CastNode.cpp nodes/CastNode.h nodes/MethodNode.cpp nodes/MethodNode.h logger/Logger.cpp logger/Logger.h nodes/DefaultConstructorNode.h nodes/DefaultConstructorNode.cpp nodes/DefaultConstructorNode.h simple_nodes/FunctionSNode.cpp simple_nodes/FunctionSNode.h simple_nodes/FunctionSNode.cpp simple_nodes/FunctionSNode.h simple_nodes/BlockSNode.cpp simple_nodes/BlockSNode.h simple_nodes/DeclarationSNode.cpp simple_nodes/DeclarationSNode.h simple_nodes/AssignmentSNode.cpp simple_nodes/AssignmentSNode.h simple_nodes/IdSNode.cpp simple_nodes/IdSNode.h simple_nodes/SNode.cpp simple_nodes/SNode.h simple_nodes/ReturnSNode.cpp simple_nodes/ReturnSNode.h simple_nodes/IntegerSNode.cpp simple_nodes/IntegerSNode.h simple_nodes/CallSNode.cpp simple_nodes/CallSNode.h simple_nodes/StringSNode.cpp simple_nodes/StringSNode.h simple_nodes/BoolSNode.cpp simple_nodes/BoolSNode.h simple_nodes/FloatSNode.cpp simple_nodes/FloatSNode.h simple_nodes/ClassSNode.cpp simple_nodes/ClassSNode.h simple_nodes/NewObjectSNode.cpp simple_nodes/NewObjectSNode.h simple_nodes/ObjectMemberSNode.cpp simple_nodes/ObjectMemberSNode.h simple_nodes/WhileSNode.cpp simple_nodes/ListSNode.cpp simple_nodes/ListSNode.h simple_nodes/IfSNode.cpp simple_nodes/IfSNode.h)

add_library(parser parser/Parser.cpp parser/Parser.h parser/ParserErrors.cpp)

add_library(
        srcLib
        logging/logging.cpp
        semantic/SymbolTable.cpp
        semantic/SymbolTable.h
        semantic/GlobalProcessor.cpp
        utils.h
        utils.cpp
        semantic/SemanticInfo.cpp
        semantic/SemanticInfo.h
        units/Class.cpp
        units/Class.h
        semantic/Checker.cpp
        semantic/Checker.h
        nodes/node_types.h
        types.h
        macros.h
        semantic/CheckerErrors.cpp
        semantic/CheckerVisit.cpp
        semantic/CheckerUnify.cpp semantic/util.cpp semantic/util.h units/Package.cpp units/Package.h units/Module.cpp units/Module.h units/ConstFunction.h units/Entity.cpp units/Entity.h units/ConstFunction.cpp units/ObjectValue.cpp units/ObjectValue.h units/FunctionValue.cpp units/FunctionValue.h semantic/ErrorReporter.cpp semantic/ErrorReporter.h)

add_library(transpiler transpiler/Transpiler.cpp transpiler/Transpiler.h transpiler/STranspiler.cpp transpiler/STranspiler.h)
