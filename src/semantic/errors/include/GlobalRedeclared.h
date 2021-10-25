//
// Created by chris on 12/8/21.
//

#ifndef XLANG_GLOBALREDECLARED_H
#define XLANG_GLOBALREDECLARED_H


#include "Error.h"
#include <ast/statements/Declaration.h>

class error::GlobalRedeclared : public error::Error {
public:
    GlobalRedeclared(std::string name);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string name;
};


#endif //XLANG_GLOBALREDECLARED_H
