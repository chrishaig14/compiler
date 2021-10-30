//
// Created by chris on 12/8/21.
//

#include "../include/PartialWrongNumArgs.h"

using namespace error;

Error* PartialWrongNumArgs::clone() const {
    return nullptr;
}

bool PartialWrongNumArgs::equal(const Error& other) const {
    return false;
}

std::string PartialWrongNumArgs::to_str(const CodeLines& code) const {
    return std::string();
}

PartialWrongNumArgs::PartialWrongNumArgs(TextPosition position): Error(ErrorType::partial_wrong_num_args) {

}
