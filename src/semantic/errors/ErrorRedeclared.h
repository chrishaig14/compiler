//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORREDECLARED_H
#define XLANG_ERRORREDECLARED_H


#include "Error.h"
#include "../../nodes/DeclarationNode.h"

class ErrorRedeclared : public Error {
public:
    ErrorRedeclared(std::string name, const DeclarationNode& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

    std::string name;
    const DeclarationNode& node;
};


#endif //XLANG_ERRORREDECLARED_H
