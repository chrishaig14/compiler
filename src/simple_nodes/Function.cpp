//
// Created by chris on 4/4/21.
//

#include "Function.h"
using namespace sem;
Function::Function(std::string identifier, VectorOfStrings params, Block* body)
        : SNode(SNodeType::FUNCTION), identifier(identifier), params(params), body(body) {
}
