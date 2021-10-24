//
// Created by chris on 1/5/21.
//

#include "../include/EnumMember.h"
#include <common/Path.h>

using namespace sem;

EnumMember::EnumMember(Path enum_path, const std::string& value) : Exp(ExpType::ENUM_MEMBER),enum_path(enum_path) {
    this->value = value;
}


bool EnumMember::equals(const Exp& o) const {
    auto& other = (const EnumMember&) o;
    return this->enum_path.as_str() == other.enum_path.as_str() && this->value == other.value;
}
