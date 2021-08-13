//
// Created by chris on 12/8/21.
//

#include "ErrorClassNoMember.h"

Error* ErrorClassNoMember::clone() const {
    return nullptr;
}

bool ErrorClassNoMember::equal(const Error& other) const {
    return false;
}

std::string ErrorClassNoMember::to_str() const {
    return std::string();
}

ErrorClassNoMember::ErrorClassNoMember(const ObjectType& type, const std::string basic_string_1, TextPosition position,
                                       const Node& node, TextPosition position_1, TextPosition position_2) {

}
