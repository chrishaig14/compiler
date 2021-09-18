//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORPARTIALWRONGNUMARGS_H
#define XLANG_ERRORPARTIALWRONGNUMARGS_H


#include "Error.h"

class ErrorPartialWrongNumArgs : public Error {
public:
    ErrorPartialWrongNumArgs(TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_ERRORPARTIALWRONGNUMARGS_H
