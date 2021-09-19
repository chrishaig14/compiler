//
// Created by chris on 5/4/21.
//

#include "../include/String.h"
using namespace sem;
String::String(const std::string& s) : Exp(ExpType::STRING), s(s) {
}

bool String::equals(const Exp& o) const {
    auto& other = (const String&) o;
    return this->s == other.s;
}
