//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATIONSNODE_H
#define XLANG_DECLARATIONSNODE_H

#include <string>
#include "SNode.h"

class sem::DeclarationSNode : public sem::SNode {
public:
    std::string identifier;
    USNode expression;

    DeclarationSNode(std::string identifier, USNode& expression);
};


#endif //XLANG_DECLARATIONSNODE_H
