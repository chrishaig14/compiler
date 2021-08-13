//
// Created by chris on 12/8/21.
//

#include "ErrorFunctionCallNumArgs.h"

Error* ErrorFunctionCallNumArgs::clone() const {
    return nullptr;
}

bool ErrorFunctionCallNumArgs::equal(const Error& other) const {
    return false;
}

std::string ErrorFunctionCallNumArgs::to_str() const {
    return std::string();
}

ErrorFunctionCallNumArgs::ErrorFunctionCallNumArgs(FunctionType type, TextPosition position) {

}
