//
// Created by chris on 12/8/21.
//

#include "ErrorEnumNoValue.h"

Error* ErrorEnumNoValue::clone() const {
    return nullptr;
}

bool ErrorEnumNoValue::equal(const Error& other) const {
    return false;
}

std::string ErrorEnumNoValue::to_str() const {
    return std::string();
}

ErrorEnumNoValue::ErrorEnumNoValue(std::string basic_string_1, const std::string basic_string_2, const MemberNode& node,
                                   Enum* p_enum) {

}
