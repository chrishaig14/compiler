//
// Created by chris on 12/8/21.
//

#ifndef XLANG_PARTIALWRONGNUMARGS_H
#define XLANG_PARTIALWRONGNUMARGS_H


#include "Error.h"

class error::PartialWrongNumArgs : public error::Error {
public:
    PartialWrongNumArgs(TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_PARTIALWRONGNUMARGS_H
