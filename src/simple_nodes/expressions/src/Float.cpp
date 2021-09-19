//
// Created by chris on 11/4/21.
//

#include "../include/Float.h"

using namespace sem;

Float::Float(const std::string& s) : sem::Exp(ExpType::FLOAT) {
    this->str = s;
}

bool Float::equals(const Exp& o) const {
    auto& other = (const Float&) o;
    return this->str == other.str;
}
