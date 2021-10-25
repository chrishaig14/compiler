//
// Created by chris on 12/8/21.
//

#ifndef XLANG_NOTDECLARED_H
#define XLANG_NOTDECLARED_H


#include "Error.h"
#include <ast/expressions/include/Id.h>

class error::NotDeclared : public error::Error {
    const ast::Id& idn;
public:
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    explicit NotDeclared(const ast::Id& idn);
};


#endif //XLANG_NOTDECLARED_H
