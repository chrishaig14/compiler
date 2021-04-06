//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONSNODE_H
#define XLANG_FUNCTIONSNODE_H

#include <string>
#include "SNode.h"
#include "../types.h"
#include "BlockSNode.h"

class FunctionSNode : public SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    BlockSNode* body;

    FunctionSNode();
};


#endif //XLANG_FUNCTIONSNODE_H
