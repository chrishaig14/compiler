//
// Created by chris on 8/8/21.
//

#ifndef XLANG_ERROR_H
#define XLANG_ERROR_H


#include <string>
#include <units/entities/Entity.h>
#include <units/infos/Package.h>
#include <units/infos/Enum.h>
#include <logging/logging.h>


std::string entity_to_string(const Entity& entity);

namespace error {
    class Error {
    public:
        virtual ~Error() = default;
        virtual Error* clone() const = 0;
        virtual bool equal(const Error& other) const = 0;

        bool operator==(const Error& other) const;

        virtual std::string to_str(const CodeLines& code) const = 0;


    };

    class ErrorBadReturn;

    class ErrorBoolOp;

    class ErrorCantAssign;

    class ErrorClassNoMember;

    class ErrorClassNoMethodForOp;

    class ErrorEnumNoValue;

    class ErrorExpectedExpression;

    class ErrorFor;

    class ErrorFunctionCallNumArgs;

    class ErrorFunctionReturnLastStmt;

    class ErrorGlobalRedeclared;

    class ErrorListLiteral;

    class ErrorNoMember;

    class ErrorNoMemberSuggestions;

    class ErrorNotAFunction;

    class ErrorNotDeclared;

    class ErrorObjectNoSpecialMethod;

    class ErrorPackageNoMember;

    class ErrorPartialWrongNumArgs;

    class ErrorRedeclared;

    class ErrorReporter;

    class ErrorTypeMismatch;

    class ErrorUnusedReturnValue;

    class ErrorReporter;
}


#endif //XLANG_ERROR_H
