//
// Created by chris on 4/4/21.
//

#include <iostream>
#include "../include/Integer.h"

using namespace sem;

Integer::Integer(const std::string& str) : Exp(ExpType::INTEGER), str(str) {
}

bool Integer::equals(const Exp& o) const {
    auto& other = (const Integer&) o;
    return this->str == other.str;
}

UExp Integer::clone() const {
    return std::make_unique<Integer>(*this);
}
