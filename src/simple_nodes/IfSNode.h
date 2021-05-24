//
// Created by chris on 13/4/21.
//

#ifndef XLANG_IFSNODE_H
#define XLANG_IFSNODE_H


#include "SNode.h"
#include "BlockSNode.h"

class IfSNode : public SNode {
public:
    SNode* condition;
    BlockSNode* then;
    BlockSNode* _else;
    std::vector<std::pair<SNode*, BlockSNode*>> elifs;

    IfSNode(SNode* condition, BlockSNode* then, std::vector<std::pair<SNode*, BlockSNode*>> elifs, BlockSNode* _else);
};


#endif //XLANG_IFSNODE_H
