cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

add_library(
        semantic
        CheckCall.cpp
        CheckCall.h
        CheckDeclaration.cpp
        CheckDeclaration.h
        ModuleChecker.cpp
        ModuleChecker.h
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
        errors/include/Error.h
        errors/include/BadReturn.h
        errors/include/BoolOp.h
        errors/include/CantAssign.h
        errors/include/ClassNoMember.h
        errors/include/ClassNoMethodForOp.h
        errors/include/EnumNoValue.h
        errors/include/ExpectedExpression.h
        errors/include/For.h
        errors/include/FunctionCallNumArgs.h
        errors/include/FunctionReturnLastStmt.h
        errors/include/NoMember.h
        errors/include/NoMemberSuggestions.h
        errors/include/NotAFunction.h
        errors/include/NotDeclared.h
        errors/include/ObjectNoSpecialMethod.h
        errors/include/PackageNoMember.h
        errors/include/PartialWrongNumArgs.h
        errors/include/GlobalRedeclared.h
        errors/include/Redeclared.h
        errors/include/ErrorReporter.h
        errors/include/errors.h
        errors/include/TypeMismatch.h
        errors/include/UnusedReturnValue.h
        errors/src/Error.cpp
        errors/src/BadReturn.cpp
        errors/src/BoolOp.cpp
        errors/src/CantAssign.cpp
        errors/src/ClassNoMember.cpp
        errors/src/ClassNoMethodForOp.cpp
        errors/src/EnumNoValue.cpp
        errors/src/ExpectedExpression.cpp
        errors/src/For.cpp
        errors/src/FunctionCallNumArgs.cpp
        errors/src/FunctionReturnLastStmt.cpp
        errors/src/NoMember.cpp
        errors/src/NoMemberSuggestions.cpp
        errors/src/NotAFunction.cpp
        errors/src/NotDeclared.cpp
        errors/src/ObjectNoSpecialMethod.cpp
        errors/src/PackageNoMember.cpp
        errors/src/PartialWrongNumArgs.cpp
        errors/src/Redeclared.cpp
        errors/src/GlobalRedeclared.cpp
        errors/src/ErrorReporter.cpp
        errors/src/TypeMismatch.cpp
        errors/src/UnusedReturnValue.cpp
        errors/src/ErrorFormatter.cpp
        errors/include/ErrorFormatter.h
        ExpressionInfo.cpp
        ExpressionInfo.h
        ModulePrechecker.cpp
        SymbolTable.cpp
        SymbolTable.h
        util.cpp
        util.h
        errors/src/ErrorFormatter.cpp
        my_error_formatter/MyErrorFormatter.cpp
        my_error_formatter/MyErrorFormatter.h
        errors/include/all_errors.h errors/include/TypeclassNotFound.cpp errors/include/TypeclassNotFound.h)

target_link_libraries(semantic units simple_nodes)
