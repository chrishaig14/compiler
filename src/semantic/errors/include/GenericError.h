//
// Created by chris on 30/10/21.
//

#ifndef XLANG_GENERICERROR_H
#define XLANG_GENERICERROR_H

#include "Error.h"

class error::GenericError : public error::Error {
public:
    TextPosition start;

    GenericError(const std::string& msg, TextPosition start) : Error(ErrorType::generic_error), start(start), msg(msg) {
    }

    const std::string& msg;
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_GENERICERROR_H
