//
// Created by chris on 12/8/21.
//

#include "../include/NotDeclared.h"

using namespace error;

Error* NotDeclared::clone() const {
    return new NotDeclared(this->idn);
}

bool NotDeclared::equal(const Error& other) const {
    auto& o = (const NotDeclared&) other;
    return this->idn == o.idn;
}

std::string NotDeclared::to_str(const CodeLines& code) const {
    return "Error not declared: " + this->idn._id;
}

NotDeclared::NotDeclared(const ast::Id& idn) : idn(idn) {
}
