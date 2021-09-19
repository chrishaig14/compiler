//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATION_H
#define XLANG_DECLARATION_H

#include <string>
#include "SNode.h"
#include <cassert>

class sem::Declaration : public sem::SNode {
    UExp _expression;
public:
    std::string identifier;
    Exp& expression;

    Declaration(const std::string& identifier, UExp _expression);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_DECLARATION_H
