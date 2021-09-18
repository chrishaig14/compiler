//
// Created by chris on 12/8/21.
//

#include "ErrorCantAssign.h"

Error* ErrorCantAssign::clone() const {
    return nullptr;
}

bool ErrorCantAssign::equal(const Error& other) const {
    return false;
}

std::string ErrorCantAssign::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorCantAssign::ErrorCantAssign(const ast::Node& n) : n(n) {
}
