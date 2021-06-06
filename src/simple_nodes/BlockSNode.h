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
    std::vector<std::string> locals;
    std::vector<SNode*> nodes;

    BlockSNode();
};


#endif //XLANG_BLOCKSNODE_H
