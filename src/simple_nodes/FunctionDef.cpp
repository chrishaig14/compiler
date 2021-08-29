//
// Created by chris on 4/4/21.
//

#include "FunctionDef.h"
using namespace sem;
FunctionDef::FunctionDef(std::string identifier, VectorOfStrings params, Block* body)
        : SNode(SNodeType::FUNCTION), identifier(identifier), params(params), body(body) {
}
