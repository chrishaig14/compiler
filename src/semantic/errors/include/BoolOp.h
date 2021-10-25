//
// Created by chris on 12/8/21.
//

#ifndef XLANG_BOOLOP_H
#define XLANG_BOOLOP_H


#include "Error.h"

class error::BoolOp : public error::Error {
public:
    BoolOp(const Entity& left, const Entity& right, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_BOOLOP_H
