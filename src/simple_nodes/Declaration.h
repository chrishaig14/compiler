//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATION_H
#define XLANG_DECLARATION_H

#include <string>
#include "SNode.h"

class sem::Declaration : public sem::SNode {
public:
    std::string identifier;
    USNode expression;

    Declaration(std::string identifier, USNode& expression);
};


#endif //XLANG_DECLARATION_H
