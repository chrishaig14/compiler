//
// Created by chris on 4/4/21.
//

#ifndef XLANG_BLOCKSNODE_H
#define XLANG_BLOCKSNODE_H

#include "SNode.h"
#include <vector>
#include <string>

class BlockSNode : public SNode {
public:
    bool unwrap;
    std::vector<std::string> locals;
    std::vector<SNode*> nodes;
    BlockSNode(bool unwrap);
    BlockSNode();
};


#endif //XLANG_BLOCKSNODE_H
