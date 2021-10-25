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

    enum class ErrorType {
        bad_return, type_mismatch, missing
    };

    class Error {
    public:
        const ErrorType error_type;

        Error(ErrorType error_type) : error_type(error_type) {
        }

        Error() : error_type(ErrorType::missing) {
        }

        virtual ~Error() = default;
        virtual Error* clone() const = 0;
        virtual bool equal(const Error& other) const = 0;

        bool operator==(const Error& other) const;

        virtual std::string to_str(const CodeLines& code) const = 0;


    };

    class ErrorReporter;

    class BadReturn;

    class BoolOp;

    class CantAssign;

    class ClassNoMember;

    class ClassNoMethodForOp;

    class EnumNoValue;

    class ExpectedExpression;

    class For;

    class FunctionCallNumArgs;

    class FunctionReturnLastStmt;

    class GlobalRedeclared;

    class ListLiteral;

    class NoMember;

    class NoMemberSuggestions;

    class NotAFunction;

    class NotDeclared;

    class ObjectNoSpecialMethod;

    class PackageNoMember;

    class PartialWrongNumArgs;

    class Redeclared;

    class TypeMismatch;

    class UnusedReturnValue;

    class TypeclassNotFound;
}


#endif //XLANG_ERROR_H
