//
// Created by chris on 12/8/21.
//

#include "ErrorBoolOp.h"

Error* ErrorBoolOp::clone() const {
    return nullptr;
}

bool ErrorBoolOp::equal(const Error& other) const {
    return false;
}

std::string ErrorBoolOp::to_str() const {
    return std::string();
}

ErrorBoolOp::ErrorBoolOp(const Entity& left, const Entity& right, TextPosition position) {

}
