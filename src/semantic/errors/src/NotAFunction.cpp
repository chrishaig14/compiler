//
// Created by chris on 12/8/21.
//

#include "../include/NotAFunction.h"

using namespace error;

Error* NotAFunction::clone() const {
    return nullptr;
}

bool NotAFunction::equal(const Error& other) const {
    return false;
}

std::string NotAFunction::to_str(const CodeLines& code) const {
    return std::string();
}

NotAFunction::NotAFunction(const ast::Call& node) {

}
