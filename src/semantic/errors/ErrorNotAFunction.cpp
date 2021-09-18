//
// Created by chris on 12/8/21.
//

#include "ErrorNotAFunction.h"

Error* ErrorNotAFunction::clone() const {
    return nullptr;
}

bool ErrorNotAFunction::equal(const Error& other) const {
    return false;
}

std::string ErrorNotAFunction::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorNotAFunction::ErrorNotAFunction(const ast::Call& node) {

}
