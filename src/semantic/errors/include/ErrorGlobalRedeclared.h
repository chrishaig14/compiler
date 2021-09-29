//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORGLOBALREDECLARED_H
#define XLANG_ERRORGLOBALREDECLARED_H


#include "Error.h"
#include "../../../ast/statements/Declaration.h"

class ErrorGlobalRedeclared : public Error {
public:
    ErrorGlobalRedeclared(std::string name);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string name;
};


#endif //XLANG_ERRORGLOBALREDECLARED_H
