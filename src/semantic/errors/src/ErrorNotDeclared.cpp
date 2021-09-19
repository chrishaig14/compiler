//
// Created by chris on 12/8/21.
//

#include "../include/ErrorNotDeclared.h"

Error* ErrorNotDeclared::clone() const {
    return new ErrorNotDeclared(this->idn);
}

bool ErrorNotDeclared::equal(const Error& other) const {
    auto& o = (const ErrorNotDeclared&) other;
    return this->idn == o.idn;
}

std::string ErrorNotDeclared::to_str(const CodeLines& code) const {
    return "Error not declared: " + this->idn._id;
}

ErrorNotDeclared::ErrorNotDeclared(const ast::Id& idn) : idn(idn) {
}
