//
// Created by chris on 11/4/21.
//

#include "../include/Option.h"

using namespace sem;

Option::Option(sem::UExp exp) : sem::Exp(ExpType::OPTION), exp(std::move(exp)) {
}

bool Option::equals(const Exp& o) const {
    auto& other = (const Option&) o;
    return this->exp == other.exp;
}

UExp Option::clone() const {
    return std::make_unique<Option>(*this);
}
