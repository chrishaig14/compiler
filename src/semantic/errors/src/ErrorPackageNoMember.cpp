//
// Created by chris on 12/8/21.
//

#include "../include/ErrorPackageNoMember.h"

Error* ErrorPackageNoMember::clone() const {
    return nullptr;
}

bool ErrorPackageNoMember::equal(const Error& other) const {
    return false;
}

std::string ErrorPackageNoMember::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorPackageNoMember::ErrorPackageNoMember(Package* p_package, const std::string basic_string_1, TextPosition position,
                                           const ast::CommonNode& node, TextPosition position_1, TextPosition position_2) {

}
