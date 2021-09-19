//
// Created by chris on 12/8/21.
//

#include "../include/ErrorFunctionReturnLastStmt.h"

Error* ErrorFunctionReturnLastStmt::clone() const {
    return nullptr;
}

bool ErrorFunctionReturnLastStmt::equal(const Error& other) const {
    return false;
}

std::string ErrorFunctionReturnLastStmt::to_str(const CodeLines& code) const {
    return std::string();
}

ErrorFunctionReturnLastStmt::ErrorFunctionReturnLastStmt(std::string name, const sem::Type& node,
                                                         TextPosition position) {

}
