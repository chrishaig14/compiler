//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORNOTDECLARED_H
#define XLANG_ERRORNOTDECLARED_H


#include "Error.h"
#include "../../ast/IdNode.h"

class ErrorNotDeclared : public Error {
    const ast::IdNode& idn;
public:
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

    explicit ErrorNotDeclared(const ast::IdNode& idn);
};


#endif //XLANG_ERRORNOTDECLARED_H
