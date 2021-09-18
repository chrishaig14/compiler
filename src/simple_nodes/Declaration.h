//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATION_H
#define XLANG_DECLARATION_H

#include <string>
#include "SNode.h"
#include <cassert>

class sem::Declaration : public sem::SNode {
    USNode _expression;
public:
    std::string identifier;
    sem::SNode& expression;

    Declaration(const std::string& identifier, USNode _expression);

    bool equals(const SNode& o) const override;
};


#endif //XLANG_DECLARATION_H
