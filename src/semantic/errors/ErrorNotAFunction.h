//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORNOTAFUNCTION_H
#define XLANG_ERRORNOTAFUNCTION_H


#include "Error.h"

class ErrorNotAFunction : public Error {
public:
    ErrorNotAFunction(const CallNode& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

};


#endif //XLANG_ERRORNOTAFUNCTION_H
