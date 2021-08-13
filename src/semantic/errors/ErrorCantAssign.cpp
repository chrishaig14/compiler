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

std::string ErrorCantAssign::to_str() const {
    return std::string();
}

ErrorCantAssign::ErrorCantAssign(const Node& n) : n(n) {
}
