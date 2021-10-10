//
// Created by chris on 12/8/21.
//

#include "../include/ErrorFunctionCallNumArgs.h"

Error* ErrorFunctionCallNumArgs::clone() const {
    return nullptr;
}

bool ErrorFunctionCallNumArgs::equal(const Error& other) const {
    return false;
}

std::string ErrorFunctionCallNumArgs::to_str(const CodeLines& code) const {
    return "Error, calling function with wrong number of arguments";
}

ErrorFunctionCallNumArgs::ErrorFunctionCallNumArgs(const sem::TypeFunction* type, TextPosition position) {

}
