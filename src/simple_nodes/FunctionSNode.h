//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONSNODE_H
#define XLANG_FUNCTIONSNODE_H

#include <string>
#include "SNode.h"
#include "../types.h"
#include "Block.h"

class FunctionSNode : public SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    Block* body;

    FunctionSNode(std::string identifier, VectorOfStrings params, Block* body);
};


#endif //XLANG_FUNCTIONSNODE_H
