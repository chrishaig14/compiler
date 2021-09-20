//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORREDECLARED_H
#define XLANG_ERRORREDECLARED_H


#include "Error.h"
#include "../../../ast/statements/Declaration.h"

class ErrorRedeclared : public Error {
public:
    ErrorRedeclared(std::string name, const ast::Declaration& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string name;
    const ast::Declaration& node;
};


#endif //XLANG_ERRORREDECLARED_H
