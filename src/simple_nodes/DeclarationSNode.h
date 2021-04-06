//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATIONSNODE_H
#define XLANG_DECLARATIONSNODE_H

#include <string>
#include "SNode.h"

class DeclarationSNode : public SNode {
public:
    std::string identifier;
    SNode* expression;

    DeclarationSNode();
};


#endif //XLANG_DECLARATIONSNODE_H
