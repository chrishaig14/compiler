//
// Created by chris on 1/5/21.
//

#include "../include/EnumMember.h"

using namespace sem;

EnumMember::EnumMember(const std::string& enum_name, const std::string& value) : Exp(ExpType::ENUM_MEMBER) {
    this->enum_name = enum_name;
    this->value = value;
}


bool EnumMember::equals(const Exp& o) const {
    auto& other = (const EnumMember&) o;
    return this->enum_name == other.enum_name && this->value == other.value;
}
