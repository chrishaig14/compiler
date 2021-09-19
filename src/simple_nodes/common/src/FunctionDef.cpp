//
// Created by chris on 4/4/21.
//

#include "../include/FunctionDef.h"

using namespace sem;

FunctionDef::FunctionDef(std::string identifier, VectorOfStrings params, std::unique_ptr<Block> body)
        : SNode(SNodeType::FUNCTION), identifier(identifier), params(params), body(std::move(body)) {
}
