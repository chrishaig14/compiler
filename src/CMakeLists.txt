cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 14)

add_library(tinyxml tinyxml/tinyxml2.cpp tinyxml/tinyxml2.h)

add_library(scanner scanner/Scanner.cpp scanner/Scanner.h scanner/Token.cpp
        scanner/Token.h scanner/CodeLines.cpp scanner/CodeLines.h scanner/TextPosition.cpp scanner/TextPosition.h)

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
        nodes/ClassLiteralExpressionNode.cpp
        nodes/ClassLiteralExpressionNode.h
        nodes/EmptyListNode.cpp
        nodes/EmptyListNode.h
        nodes/ClassLiteralFieldNode.cpp
        nodes/ClassLiteralFieldNode.h
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
        nodes/FloatNode.cpp
        nodes/FloatNode.h
        nodes/BoolOpNode.cpp
        nodes/BoolOpNode.h
        nodes/PartialApplication.cpp
        nodes/PartialApplication.h nodes/DictNode.cpp nodes/DictNode.h nodes/EmptyDictNode.cpp nodes/EmptyDictNode.h)

add_library(parser parser/Parser.cpp parser/Parser.h parser/UnexpectedToken.cpp
            parser/UnexpectedToken.h parser/ParserErrors.cpp)

add_library(
        srcLib
        logging/logging.cpp
        semantic/ClassTable.cpp
        semantic/ClassTable.h
        semantic/SymbolTable.cpp
        semantic/SymbolTable.h
        semantic/GlobalProcessor.cpp
        utils.h
        utils.cpp
        semantic/SemanticInfo.cpp
        semantic/SemanticInfo.h
        semantic/ObjectInfo.cpp
        semantic/ObjectInfo.h
        semantic/ClassInfo.cpp
        semantic/ClassInfo.h
        semantic/FunctionInfo.cpp
        semantic/FunctionInfo.h
        semantic/Checker.cpp
        semantic/Checker.h
        semantic/RedeclareError.cpp
        semantic/RedeclareError.h
        semantic/BadArguments.cpp
        semantic/BadArguments.h
        semantic/ReturnError.cpp
        semantic/ReturnError.h
        semantic/ScopeError.cpp
        semantic/ScopeError.h
        vm/Object.h
        vm/Object.cpp
        vm/CodeObject.cpp
        vm/CodeObject.h
        vm/CodeUser.cpp
        vm/CodeUser.h
        vm/IntegerObject.cpp
        vm/IntegerObject.h
        vm/Environment.h
        vm/Environment.cpp
        vm/UserObject.cpp
        vm/UserObject.h
        vm/ListObject.cpp
        vm/ListObject.h
        vm/BooleanObject.cpp
        vm/BooleanObject.h
        vm/NoneObject.cpp
        vm/NoneObject.h
        semantic/TypeClassInfo.cpp
        semantic/TypeClassInfo.h
        nodes/node_types.h
        types.h
        macros.h
        vm/TupleObject.cpp
        vm/TupleObject.h
        vm/FloatObject.cpp
        vm/FloatObject.h
        vm/ObjectStore.cpp
        vm/ObjectStore.h
        semantic/CheckerErrors.cpp semantic/CheckerVisit.cpp semantic/CheckerUnify.cpp)

add_library(transpiler transpiler/Transpiler.cpp transpiler/Transpiler.h)
