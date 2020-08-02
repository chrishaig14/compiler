//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

void TypeNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

TypeNode::TypeNode(const std::string& identifier, const std::vector<TypeNode*>& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {}
