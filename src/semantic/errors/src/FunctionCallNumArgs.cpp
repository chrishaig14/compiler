//
// Created by chris on 12/8/21.
//

#include "../include/FunctionCallNumArgs.h"

using namespace error;

Error* FunctionCallNumArgs::clone() const {
    return nullptr;
}

bool FunctionCallNumArgs::equal(const Error& other) const {
    return false;
}

std::string FunctionCallNumArgs::to_str(const CodeLines& code) const {
    return "Error, calling function with wrong number of arguments";
}

FunctionCallNumArgs::FunctionCallNumArgs(const sem::TypeFunction* type, TextPosition position) {

}
