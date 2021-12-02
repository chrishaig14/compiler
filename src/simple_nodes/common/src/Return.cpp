//
// Created by chris on 4/4/21.
//

#include "../include/Return.h"
#include "../../expressions/include/Exp.h"

using namespace sem;

Return::Return(UExp expression) : Common(CommonType::RETURN), expression(std::move(expression)) {
}

bool Return::equals(const Common& o) const {
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

std::unique_ptr<Common> Return::clone() const {
    return std::make_unique<Return>(*this);
}

Return::Return(const Return& other) : sem::Common(CommonType::RETURN) {
    this->expression = other.expression->clone();
    this->reachables = other.reachables;
}
