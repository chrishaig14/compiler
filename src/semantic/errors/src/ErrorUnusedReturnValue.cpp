//
// Created by chris on 12/8/21.
//

#include "../include/ErrorUnusedReturnValue.h"

Error* ErrorUnusedReturnValue::clone() const {
    return nullptr;
}

bool ErrorUnusedReturnValue::equal(const Error& other) const {
    return false;
}

std::string ErrorUnusedReturnValue::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorUnusedReturnValue::ErrorUnusedReturnValue(TextPosition position) {

}
