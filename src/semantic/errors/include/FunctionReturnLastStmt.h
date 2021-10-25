//
// Created by chris on 12/8/21.
//

#ifndef XLANG_FUNCTIONRETURNLASTSTMT_H
#define XLANG_FUNCTIONRETURNLASTSTMT_H


#include "Error.h"

class error::FunctionReturnLastStmt : public error::Error {
public:
    FunctionReturnLastStmt(std::string name, const sem::Type& node, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_FUNCTIONRETURNLASTSTMT_H
