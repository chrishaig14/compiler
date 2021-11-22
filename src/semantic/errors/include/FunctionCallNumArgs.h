//
// Created by chris on 12/8/21.
//

#ifndef XLANG_FUNCTIONCALLNUMARGS_H
#define XLANG_FUNCTIONCALLNUMARGS_H


#include "Error.h"

class error::FunctionCallNumArgs : public error::Error {
public:
    TextPosition start;
    TextPosition end;

    const sem::TypeFunction* type;

    FunctionCallNumArgs(const sem::TypeFunction* type, const ast::ExpNode& node);

    Error* clone() const override;
    bool equal(const Error& other) const override;

    std::string to_str(const CodeLines& code) const override;
};


#endif //XLANG_FUNCTIONCALLNUMARGS_H
