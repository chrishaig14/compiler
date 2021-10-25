//
// Created by chris on 12/8/21.
//

#ifndef XLANG_REDECLARED_H
#define XLANG_REDECLARED_H


#include "Error.h"
#include <ast/statements/Declaration.h>

class error::Redeclared : public error::Error {
public:
    Redeclared(std::string name, const ast::Declaration& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string name;
    const ast::Declaration& node;
};


#endif //XLANG_REDECLARED_H
