cmake_minimum_required(VERSION 3.16)
project(xlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -Wall -pedantic -Werror")

add_library(
        semantic
        ../util/macros.h
        ../util/types.h
        ../util/utils.cpp
        ../util/utils.h
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
        errors/src/ErrorReporter.cpp
        errors/include/ErrorReporter.h
        errors/src/ErrorTypeMismatch.cpp
        errors/include/ErrorTypeMismatch.h
        GlobalProcessor.cpp
        SymbolTable.cpp
        SymbolTable.h
        util.cpp
        util.h errors/src/Error.cpp errors/include/Error.h errors/src/ErrorNoMember.cpp errors/include/ErrorNoMember.h errors/src/ErrorNoMemberSuggestions.cpp errors/include/ErrorNoMemberSuggestions.h errors/src/ErrorNotDeclared.cpp errors/include/ErrorNotDeclared.h errors/src/ErrorNotAFunction.cpp errors/include/ErrorNotAFunction.h errors/src/ErrorFunctionCallNumArgs.cpp errors/include/ErrorFunctionCallNumArgs.h errors/src/ErrorExpectedExpression.cpp errors/include/ErrorExpectedExpression.h errors/src/ErrorRedeclared.cpp errors/include/ErrorRedeclared.h errors/src/ErrorEnumNoValue.cpp errors/include/ErrorEnumNoValue.h errors/src/ErrorListLiteral.cpp errors/include/ErrorListLiteral.h errors/src/ErrorCantAssign.cpp errors/include/ErrorCantAssign.h errors/src/ErrorBadReturn.cpp errors/include/ErrorBadReturn.h errors/src/ErrorBoolOp.cpp errors/include/ErrorBoolOp.h errors/src/ErrorClassNoMethodForOp.cpp errors/include/ErrorClassNoMethodForOp.h errors/src/ErrorUnusedReturnValue.cpp errors/include/ErrorUnusedReturnValue.h errors/src/ErrorFunctionReturnLastStmt.cpp errors/include/ErrorFunctionReturnLastStmt.h errors/src/ErrorObjectNoSpecialMethod.cpp errors/include/ErrorObjectNoSpecialMethod.h errors/src/ErrorFor.cpp errors/include/ErrorFor.h errors/src/ErrorPartialWrongNumArgs.cpp errors/include/ErrorPartialWrongNumArgs.h errors/src/ErrorClassNoMember.cpp errors/include/ErrorClassNoMember.h errors/src/ErrorPackageNoMember.cpp errors/include/ErrorPackageNoMember.h errors/include/errors.h ExpressionInfo.cpp ExpressionInfo.h)

target_link_libraries(semantic units simple_nodes)
