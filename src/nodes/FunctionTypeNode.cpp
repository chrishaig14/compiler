//
// Created by chris on 15/11/20.
//

#include "FunctionTypeNode.h"

FunctionTypeNode::FunctionTypeNode(const std::vector<TypeNode>& parameterTypes,
                                   TypeNode returnType)
        : parameter_types(parameterTypes), return_type(returnType) {}
