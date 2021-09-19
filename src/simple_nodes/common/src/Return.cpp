//
// Created by chris on 4/4/21.
//

#include "../include/Return.h"
#include "../../expressions/include/Exp.h"

using namespace sem;

Return::Return(UExp expression) : SNode(SNodeType::RETURN), expression(std::move(expression)) {
}

bool Return::equals(const SNode& o) const {
    auto& other = (const Return&) o;
    if (this->expression == nullptr && other.expression != nullptr) {
        return false;
    }
    if (this->expression != nullptr && other.expression == nullptr) {
        return false;
    }
    if (this->expression == nullptr && other.expression == nullptr) {
        return true;
    }
    return *this->expression == *other.expression;
}
