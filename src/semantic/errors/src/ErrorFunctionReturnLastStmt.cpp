//
// Created by chris on 12/8/21.
//

#include "../include/ErrorFunctionReturnLastStmt.h"

using namespace error;

Error* ErrorFunctionReturnLastStmt::clone() const {
    return nullptr;
}

bool ErrorFunctionReturnLastStmt::equal(const Error& other) const {
    return false;
}

std::string ErrorFunctionReturnLastStmt::to_str(const CodeLines& code) const {
    return "Error missing return in function";
}

ErrorFunctionReturnLastStmt::ErrorFunctionReturnLastStmt(std::string name, const sem::Type& node,
                                                         TextPosition position) {

}
