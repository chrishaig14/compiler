//
// Created by chris on 12/8/21.
//

#include "ErrorFunctionReturnLastStmt.h"

Error* ErrorFunctionReturnLastStmt::clone() const {
    return nullptr;
}

bool ErrorFunctionReturnLastStmt::equal(const Error& other) const {
    return false;
}

std::string ErrorFunctionReturnLastStmt::to_str() const {
    return std::string();
}

ErrorFunctionReturnLastStmt::ErrorFunctionReturnLastStmt(std::string name, const ast::TypeNode& node,
                                                         TextPosition position) {

}
