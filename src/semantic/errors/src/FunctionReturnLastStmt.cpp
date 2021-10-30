//
// Created by chris on 12/8/21.
//

#include "../include/FunctionReturnLastStmt.h"

using namespace error;

Error* FunctionReturnLastStmt::clone() const {
    return nullptr;
}

bool FunctionReturnLastStmt::equal(const Error& other) const {
    return false;
}

std::string FunctionReturnLastStmt::to_str(const CodeLines& code) const {
    return "Error missing return in function";
}

FunctionReturnLastStmt::FunctionReturnLastStmt(std::string name, const sem::Type& node,
                                               TextPosition position):Error(ErrorType::function_return_last_stmt) {

}
