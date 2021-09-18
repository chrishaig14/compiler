//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORBADRETURN_H
#define XLANG_ERRORBADRETURN_H


#include "Error.h"

class ErrorBadReturn : public Error {
public:
    ErrorBadReturn(TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_ERRORBADRETURN_H
