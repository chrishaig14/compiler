//
// Created by chris on 12/8/21.
//

#include "ErrorBadReturn.h"

Error* ErrorBadReturn::clone() const {
    return nullptr;
}

bool ErrorBadReturn::equal(const Error& other) const {
    return false;
}

std::string ErrorBadReturn::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorBadReturn::ErrorBadReturn(TextPosition position) {

}
