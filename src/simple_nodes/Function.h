//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTION_H
#define XLANG_FUNCTION_H

#include <string>
#include "SNode.h"
#include "../types.h"
#include "Block.h"

class sem::Function : public sem::SNode {
public:
    std::string identifier;
    VectorOfStrings params;
    Block* body;

    Function(std::string identifier, VectorOfStrings params, Block* body);
};


#endif //XLANG_FUNCTION_H
