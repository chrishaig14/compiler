//
// Created by chris on 1/8/20.
//

#include "DeclarationNode.h"

void DeclarationNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

DeclarationNode::DeclarationNode(const std::string& identifier, TypeNode* type, NodeContainer expression) : identifier(
        identifier),
                                                                                                    type(type),
                                                                                                    expression(
                                                                                                            expression) {}
