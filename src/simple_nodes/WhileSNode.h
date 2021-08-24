//
// Created by chris on 10/4/21.
//

#ifndef XLANG_WHILESNODE_H
#define XLANG_WHILESNODE_H


#include "SNode.h"
#include "BlockSNode.h"

class WhileSNode : public SNode {
public:
    USNode condition;
    BlockSNode* body;

    WhileSNode(USNode condition, BlockSNode* body);
};


#endif //XLANG_WHILESNODE_H
