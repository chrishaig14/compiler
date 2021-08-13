//
// Created by chris on 12/8/21.
//

#include "ErrorPackageNoMember.h"

Error* ErrorPackageNoMember::clone() const {
    return nullptr;
}

bool ErrorPackageNoMember::equal(const Error& other) const {
    return false;
}

std::string ErrorPackageNoMember::to_str() const {
    return std::string();
}

ErrorPackageNoMember::ErrorPackageNoMember(Package* p_package, const std::string basic_string_1, TextPosition position,
                                           const Node& node, TextPosition position_1, TextPosition position_2) {

}
