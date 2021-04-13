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

    IfSNode() : SNode(SNodeType::IF) {
    }
};


#endif //XLANG_IFSNODE_H
