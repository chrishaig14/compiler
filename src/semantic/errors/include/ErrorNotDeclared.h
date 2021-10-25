//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORNOTDECLARED_H
#define XLANG_ERRORNOTDECLARED_H


#include "Error.h"
#include <ast/expressions/include/Id.h>

class error::ErrorNotDeclared : public error::Error {
    const ast::Id& idn;
public:
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    explicit ErrorNotDeclared(const ast::Id& idn);
};


#endif //XLANG_ERRORNOTDECLARED_H
