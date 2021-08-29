//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IFSNODE_H
#define XLANG_IFSNODE_H


#include "SNode.h"
#include "Block.h"

class IfSNode : public SNode {
public:
    SNode* condition;
    Block* then;
    Block* _else;
    std::vector<std::pair<SNode*, Block*>> elifs;

    IfSNode(SNode* condition, Block* then, std::vector<std::pair<SNode*, Block*>> elifs, Block* _else);
};


#endif //XLANG_IFSNODE_H
