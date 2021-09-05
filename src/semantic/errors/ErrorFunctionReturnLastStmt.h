//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORFUNCTIONRETURNLASTSTMT_H
#define XLANG_ERRORFUNCTIONRETURNLASTSTMT_H


#include "Error.h"

class ErrorFunctionReturnLastStmt : public Error {
public:
    ErrorFunctionReturnLastStmt(std::string name, const ast::Type& node, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

};


#endif //XLANG_ERRORFUNCTIONRETURNLASTSTMT_H
