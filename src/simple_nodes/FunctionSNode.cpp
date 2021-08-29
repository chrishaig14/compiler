//
// Created by chris on 4/4/21.
//

#include "FunctionSNode.h"

FunctionSNode::FunctionSNode(std::string identifier, VectorOfStrings params, Block* body)
        : SNode(SNodeType::FUNCTION), identifier(identifier), params(params), body(body) {
}
