//
// Created by chris on 12/8/21.
//

#ifndef XLANG_BADRETURN_H
#define XLANG_BADRETURN_H


#include "Error.h"

class error::BadReturn : public error::Error::Error {
public:
    BadReturn(TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_BADRETURN_H
