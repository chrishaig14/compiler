//
// Created by chris on 12/8/21.
//

#include "ErrorNotDeclared.h"

Error* ErrorNotDeclared::clone() const {
    return new ErrorNotDeclared(this->idn);
}

bool ErrorNotDeclared::equal(const Error& other) const {
    auto& o = (const ErrorNotDeclared&) other;
    return this->idn == o.idn;
}

std::string ErrorNotDeclared::to_str() const {
    return "Error not declared: " + this->idn._id;
}

ErrorNotDeclared::ErrorNotDeclared(const IdNode& idn) : idn(idn) {
}
