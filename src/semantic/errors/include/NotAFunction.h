//
// Created by chris on 12/8/21.
//

#ifndef XLANG_NOTAFUNCTION_H
#define XLANG_NOTAFUNCTION_H


#include "Error.h"
#include <ast/statements/Call.h>

class error::NotAFunction : public error::Error {
public:
    NotAFunction(const ast::Call& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

};


#endif //XLANG_NOTAFUNCTION_H
