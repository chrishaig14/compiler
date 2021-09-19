//
// Created by chris on 4/4/21.
//

#include "../include/Assignment.h"

using namespace sem;

Assignment::Assignment(UExp lvalue, UExp rvalue)
        : Common(CommonType::ASSIGNMENT), lvalue(std::move(lvalue)), rvalue(std::move(rvalue)) {
}

bool Assignment::equals(const Common& o) const {
    // auto& other = (const Declaration&) o;
    // return this->identifier == other.identifier && *this->expression == *other.expression;
    return false;
}
