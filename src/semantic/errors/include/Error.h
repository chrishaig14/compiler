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
        bad_return, type_mismatch, missing, typeclass_not_found, bool_op, function_call_num_args, cant_assign,
        class_no_member, enum_no_value, class_no_method_for_op, function_return_last_stmt, expected_expression,
        for_error, partial_wrong_num_args, no_member, not_a_function, no_member_suggestion, object_no_special_method,
        not_declared, global_redeclared, package_no_member, redeclared, unused_return_value
    };

    class Error {
    public:
        const ErrorType error_type;

        Error(ErrorType error_type) : error_type(error_type) {
        }

        // Error() : error_type(ErrorType::missing) {
        // }

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
