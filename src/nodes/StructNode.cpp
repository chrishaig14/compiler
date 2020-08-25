//
// Created by chris on 1/8/20.
//

#include "StructNode.h"

void StructNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

StructNode::StructNode(std::string identifier, std::vector<std::string> templateParameters,
                       std::vector<std::pair<std::string, TypeNode*>> fields) : identifier(
        identifier),
                                                                                template_parameters(
                                                                                        templateParameters),
                                                                                fields(fields) {}