//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONSNODE_H
#define XLANG_FUNCTIONSNODE_H

#include <string>
#include "SNode.h"
#include "../types.h"
#include "Block.h"

class sem::FunctionSNode : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    Block* body;

    FunctionSNode(std::string identifier, VectorOfStrings params, Block* body);
};


#endif //XLANG_FUNCTIONSNODE_H
