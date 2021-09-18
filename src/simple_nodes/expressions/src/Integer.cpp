//
// Created by chris on 4/4/21.
//

#include <iostream>
#include "../include/Integer.h"

using namespace sem;

Integer::Integer(const std::string& str) : SNode(SNodeType::INTEGER), str(str) {
}

bool Integer::equals(const SNode& o) const {
    auto& other = (const Integer&) o;
    return this->str == other.str;
}
