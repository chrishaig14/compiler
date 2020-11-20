cmake_minimum_required(VERSION 3.16)
project(untitled1)

set(CMAKE_CXX_STANDARD 14)

add_library(scanner scanner/Scanner.cpp
        scanner/Scanner.h
        scanner/Token.cpp
        scanner/Token.h)

add_library(nodes nodes/Visitor.cpp
        nodes/Visitor.h
        nodes/ReturnNode.cpp
        nodes/ReturnNode.h
        nodes/StringNode.cpp
        nodes/StringNode.h
        nodes/BreakNode.cpp nodes/BreakNode.h
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
        nodes/TernaryNode.cpp nodes/TernaryNode.h nodes/NoneNode.cpp nodes/NoneNode.h
        nodes/IfNode.h
        nodes/IdNode.cpp
        nodes/ClassNode.cpp nodes/ClassNode.h
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
        nodes/BooleanNode.cpp nodes/BooleanNode.h nodes/WhileNode.cpp nodes/WhileNode.h
        nodes/ContinueNode.cpp nodes/ContinueNode.h nodes/Node.cpp nodes/Node.h
        nodes/ForNode.cpp nodes/ForNode.h
        nodes/optypes.h nodes/optypes.cpp)

add_library(parser parser/Parser.cpp
        parser/Parser.h
        parser/UnexpectedToken.cpp
        parser/UnexpectedToken.h
        )

add_library(translator translator/Translator.cpp
        translator/Translator.h)
add_library(srcLib
        color_codes.h

        semantic/ClassTable.cpp
        semantic/ClassTable.h
        semantic/SymbolTable.cpp
        semantic/SymbolTable.h
        semantic/GlobalProcessor.cpp
        utils.h
        utils.cpp
        semantic/SymbolInfo.cpp
        semantic/SymbolInfo.h
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

        instructions/GetInst.cpp
        instructions/GetInst.h
        instructions/SetInst.cpp
        instructions/SetInst.h
        instructions/DeclareInst.cpp
        instructions/DeclareInst.h
        instructions/BinopInst.cpp
        instructions/BinopInst.h
        instructions/PushIntegerInst.cpp
        instructions/PushIntegerInst.h
        instructions/PushStringInst.cpp
        instructions/PushStringInst.h
        instructions/Instruction.cpp
        instructions/Instruction.h
        instructions/CallInst.cpp
        instructions/CallInst.h
        instructions/ReturnInst.cpp
        instructions/ReturnInst.h
        instructions/GetMemberInst.cpp
        instructions/GetMemberInst.h
        instructions/SetMemberInst.cpp
        instructions/SetMemberInst.h
        instructions/SetSubscriptInst.cpp
        instructions/SetSubscriptInst.h
        instructions/GetSubscriptInst.cpp
        instructions/GetSubscriptInst.h
        instructions/PushFunctionInst.cpp
        instructions/PushFunctionInst.h
        instructions/instructions.h
        instructions/InstructionVisitor.cpp
        instructions/InstructionVisitor.h
        vm/ObjectStack.h
        vm/ObjectStack.cpp
        vm/Object.h
        vm/Object.cpp
        vm/CodeObject.cpp
        vm/CodeObject.h
        vm/CodeUser.cpp
        vm/CodeUser.h
        vm/IntegerObject.cpp
        vm/IntegerObject.h
        vm/StringObject.cpp
        vm/StringObject.h
        vm/Environment.h
        vm/Environment.cpp
        vm/CodeRunner.h
        vm/CodeRunner.cpp
        vm/UserObject.cpp
        vm/UserObject.h
        instructions/MakeObjectInst.cpp
        instructions/MakeObjectInst.h
        instructions/MakeListInst.cpp
        instructions/MakeListInst.h
        vm/ListObject.cpp
        vm/ListObject.h
        instructions/JumpIfFalseInst.cpp
        instructions/JumpIfFalseInst.h
        instructions/PushBooleanInst.cpp
        instructions/PushBooleanInst.h
        vm/BooleanObject.cpp
        vm/BooleanObject.h
        instructions/MakeClassInst.cpp
        instructions/MakeClassInst.h


        instructions/DefineStructInst.cpp
        instructions/DefineStructInst.h
        instructions/MakeDefaultInst.cpp
        instructions/MakeDefaultInst.h
        semantic/AssignmentTypeError.cpp semantic/AssignmentTypeError.h instructions/JumpInst.cpp instructions/JumpInst.h instructions/EnterScope.cpp instructions/EnterScope.h instructions/LeaveScope.cpp instructions/LeaveScope.h vm/LabelledCode.cpp vm/LabelledCode.h instructions/NopInst.cpp instructions/NopInst.h vm/Loader.cpp vm/Loader.h instructions/StartFunction.cpp instructions/StartFunction.h instructions/EndFunction.cpp instructions/EndFunction.h formatter/Formatter.cpp formatter/Formatter.h instructions/JumpIfNone.cpp instructions/JumpIfNone.h vm/NoneObject.cpp vm/NoneObject.h instructions/PushNone.cpp instructions/PushNone.h semantic/TypeClassInfo.cpp semantic/TypeClassInfo.h run.h run.cpp instructions/PopInst.cpp instructions/PopInst.h nodes/node_types.h types.h macros.h)

