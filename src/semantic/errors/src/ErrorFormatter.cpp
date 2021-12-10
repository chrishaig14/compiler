#include "../include/ErrorFormatter.h"

std::string ErrorFormatter::format(const error::Error& err) const {
    switch (err.error_type) {
        case error::ErrorType::bad_return:
            return this->format(static_cast<const error::BadReturn&>(err));
        case error::ErrorType::type_mismatch:
            return this->format(static_cast<const error::TypeMismatch&>(err));
        case error::ErrorType::typeclass_not_found:
            return this->format(static_cast<const error::TypeclassNotFound&>(err));
        case error::ErrorType::missing:
            throw std::runtime_error("missing formatter for this error");
        case error::ErrorType::bool_op:
            break;
        case error::ErrorType::function_call_num_args:
            return this->format(static_cast<const error::FunctionCallNumArgs&>(err));
            break;
        case error::ErrorType::cant_assign:
            break;
        case error::ErrorType::class_no_member:
            return this->format(static_cast<const error::ClassNoMember&>(err));
            break;
        case error::ErrorType::enum_no_value:
            break;
        case error::ErrorType::class_no_method_for_op:
            return this->format(static_cast<const error::ClassNoMethodForOp&>(err));
        case error::ErrorType::function_return_last_stmt:
            break;
        case error::ErrorType::expected_expression:
            break;
        case error::ErrorType::for_error:
            return this->format(static_cast<const error::For&>(err));
            break;
        case error::ErrorType::partial_wrong_num_args:
            break;
        case error::ErrorType::no_member:
            return this->format(static_cast<const error::NoMember&>(err));
        case error::ErrorType::not_a_function:
            break;
        case error::ErrorType::no_member_suggestion:
            return this->format(static_cast<const error::NoMemberSuggestions&>(err));
        case error::ErrorType::object_no_special_method:
            return this->format(static_cast<const error::ObjectNoSpecialMethod&>(err));
        case error::ErrorType::not_declared:
            return this->format(static_cast<const error::NotDeclared&>(err));
        case error::ErrorType::global_redeclared:
            return this->format(static_cast<const error::GlobalRedeclared&>(err));
        case error::ErrorType::package_no_member:
            break;
        case error::ErrorType::redeclared:
            return this->format(static_cast<const error::Redeclared&>(err));
        case error::ErrorType::unused_return_value:
            break;
        case error::ErrorType::generic_error:
            return this->format(static_cast<const error::GenericError&>(err));
    }
    return "Error";
}
