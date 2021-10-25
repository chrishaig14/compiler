//
// Created by chris on 12/8/21.
//

#include "../include/ErrorPartialWrongNumArgs.h"

using namespace error;

Error* ErrorPartialWrongNumArgs::clone() const {
    return nullptr;
}

bool ErrorPartialWrongNumArgs::equal(const Error& other) const {
    return false;
}

std::string ErrorPartialWrongNumArgs::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorPartialWrongNumArgs::ErrorPartialWrongNumArgs(TextPosition position) {

}
