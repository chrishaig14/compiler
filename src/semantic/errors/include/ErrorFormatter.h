#ifndef ERROR_FORMATTER_H
#define ERROR_FORMATTER_H

#include "all_errors.h"

class ErrorFormatter {
public:
    std::string format(const error::Error& err) const;

    virtual std::string format(const error::BadReturn& err) const = 0;
    virtual std::string format(const error::TypeMismatch& err) const = 0;
    virtual std::string format(const error::TypeclassNotFound& err) const = 0;
    virtual std::string format(const error::GenericError& err) const = 0;
    virtual std::string format(const error::FunctionCallNumArgs& err) const = 0;
    virtual std::string format(const error::Redeclared& err) const = 0;
    virtual std::string format(const error::NoMember& err) const = 0;
    virtual std::string format(const error::NoMemberSuggestions& err) const = 0;
    virtual std::string format(const error::ObjectNoSpecialMethod& err) const = 0;
    virtual std::string format(const error::ClassNoMethodForOp& err) const = 0;
    virtual std::string format(const error::ClassNoMember& err) const = 0;
    virtual std::string format(const error::NotDeclared& err) const = 0;
    virtual std::string format(const error::GlobalRedeclared& err) const = 0;
    virtual std::string format(const error::For& err) const = 0;
    virtual ~ErrorFormatter() = default;
};

#endif // ERROR_FORMATTER_H