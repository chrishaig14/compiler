//
// Created by chris on 12/8/21.
//

#include "../include/PackageNoMember.h"

using namespace error;

Error* PackageNoMember::clone() const {
    return nullptr;
}

bool PackageNoMember::equal(const Error& other) const {
    return false;
}

std::string PackageNoMember::to_str(const CodeLines& code) const {
    return std::string();
}

PackageNoMember::PackageNoMember(Package* p_package, const std::string basic_string_1, TextPosition position,
                                 const ast::Statement& node, TextPosition position_1,
                                 TextPosition position_2) {

}
