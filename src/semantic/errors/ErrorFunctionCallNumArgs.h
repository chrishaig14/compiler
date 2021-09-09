//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORFUNCTIONCALLNUMARGS_H
#define XLANG_ERRORFUNCTIONCALLNUMARGS_H


#include "Error.h"

class ErrorFunctionCallNumArgs : public Error {
public:
    ErrorFunctionCallNumArgs(const sem::TypeFunction* type, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

};


#endif //XLANG_ERRORFUNCTIONCALLNUMARGS_H
