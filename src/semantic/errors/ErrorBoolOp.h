//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORBOOLOP_H
#define XLANG_ERRORBOOLOP_H


#include "Error.h"

class ErrorBoolOp : public Error {
public:
    ErrorBoolOp(const Entity& left, const Entity& right, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

};


#endif //XLANG_ERRORBOOLOP_H
